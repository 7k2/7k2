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

//Filename    : OTUTOR3.CPP
//Description : Learning Campaign related functions

#include <OFIRMALL.H>
#include <ONATIONA.H>
#include <OTUTOR.H>
#include <OSYS.H>
#include <OGAME.H>
#include <OCAMPALL.H>
#include <OGAMMENU.H>
#include <OPROFILE.H>
#include <OBITMAP.H>
#include <OVGA.H>
#include <OMODEID.H>
#include <OVGALOCK.H>
#include <OCOLTBL.H>
#include <OMOUSE.H> 
#include <OIMGRES.H>
#include <OFONT.H>
#include <OBUTTCUS.H>
#include <OGETA.H>
#include <OMUSIC.H>
#include <MATH.H>
#include <OT_GMENU.H>
#include <OT_TUTOR.H>
#include <OPOWER.H>
#include <OOPTMENU.H>
#include <OINGMENU.H>
#include <OBOX.H>
#include <OFIRM.H>
#include <OBUTT3D.H>
#include <OUN_GRP.H>
#include <OF_LAIR.H>
#include <OHERORES.H>
#include <OTECHRES.H>
#include <OFILETXT.H>
#include <OU_CARA.H>
#include <OSPY.H>
#include <OF_FORT.H>
#include <OF_SPEC.H>
#include <OF_SPY.H>
#include <KEY.H>

#define MAX_NO_OF_TUTORIAL			4
#define NO_OF_HUMAN_TUTORIAL		4
#define NO_OF_MONSTER_TUTORIAL	2

#define TLSCROLL_X1               39
#define TLSCROLL_Y1               69
#define TLSCROLL_X2               94
#define TLSCROLL_Y2              442
#define TRSCROLL_X1              704
#define TRSCROLL_Y1       LSCROLL_Y1
#define TRSCROLL_X2              759
#define TRSCROLL_Y2       LSCROLL_Y2
#define MAX_TUTORIAL_RECNO				8

//---------- Define option modes --------//

enum { OPTION_SPECIES,
		 OPTION_LEARNING_CAMPAIGN,		 
};

// ----- declare static function -------//

static void i_disp_text_button(ButtonCustom *, int repaintBody);
static void i_disp_race_button(ButtonCustom *, int repaintBody);
static void i_disp_color_button(ButtonCustom *, int repaintBody);

static int par1, par2;

// ------- Begin of function Tutor::select_learning_campaign_menu -----//
//
// select which campaign to play, set config as well
//
// return 0=cancel, >0 for campaign selected
//

int Tutor::select_learning_campaign_menu()
{
	int i;
//	int specy = 0;
//	int campaign_id = 0;
	int browseRecno = 1;
	int minTutorialRecno = 1; // 1
	int maxTutorialRecno = MAX_TUTORIAL_RECNO; // 8
	int specy_array[MAX_TUTORIAL_RECNO] = { 1, 1, 0, 0, 0, 0, 0, 0 };
	//tutorial_finish_count[0] indicates which tutorial (campaign_id) of monster has been finished
	//tutorial_finish_count[1] indicates which tutorial (campaign_id) of human has been finished
	int campaign_id_array[MAX_TUTORIAL_RECNO] = { 0, 1, 0, 1, 2, 3, 4, 5 };
	//{ campaign_id of both	 : 0, 1, 2, 3,			
	//{ cur_tutor_id human   : 1, 3, 5, 7, ..
	//{ cur_tutor_id frythan : 2, 4, 6, 8, ..
	
	//static char *learningCampaignList[] = { "Quick Start Tutorial", "Learning Campaign One", "Learning Campaign Two", "Learning Campaign Three" };//, "Fryhtan Campaign", "Fryhtan Learning Campaign", };
	char *learningCampaignList[] = 
	{
		text_tutorial.str_tutor_quick(),
		text_tutorial.str_tutor_1(),
		text_tutorial.str_tutor_2(),
		text_tutorial.str_tutor_3(),
	};
			
	int learningCampaignCount = sizeof(learningCampaignList) / sizeof(learningCampaignList[0]);

	err_when(MAX_NO_OF_TUTORIAL < learningCampaignCount);
/*		
	int tutorial_finish_count[2];
	
	// check in player profile to find which learning campaign have been finished
	for (i = 0; i < 2; i++)
#ifdef BETA
		tutorial_finish_count[i] = 9;
#else
		tutorial_finish_count[i] = (player_profile.tutorial_finish_count % (int)pow(10.0, (i+1))) / (int)pow(10.0, i);
#endif
*/
		
	int tutorial_finish_count;
	// check in player profile to find which learning campaign have been finished
#ifdef BETA
		tutorial_finish_count = 99;
#else
		tutorial_finish_count = player_profile.tutorial_finish_count;
#endif
		
	Config tempConfig = config;

	// ------ adjust config ------//

	tempConfig.default_campaign_setting();

	if( tempConfig.race_id < 0 )		// human race only
		tempConfig.race_id = 1;

	building_size = tempConfig.building_size;

	// ----------------------------//

	int optionMode = OPTION_SPECIES;

	TempGameSet tempGameSet(1);		// game_set.open_set
	TempUnitRes tempUnitRes;			// unit_res.init
	TempRaceRes tempRaceRes;			// race_res.init

	// take name from profile

	if( player_profile.is_registered() )
	{
		strcpy( tempConfig.player_name, player_profile.player_name );
	}

	//--------- initialize variable ---------//
	
	int w, h;
	int cx, cy;
	String str;
	long refreshFlag = SGOPTION_ALL;
	int retFlag = 0;

	// -------- generate palette ------//

	short colorRemapTable[0x100];
	{
		str = DIR_IMAGE;
		str += "CHOOSE.COL";

		File palFile;
		palFile.file_open(str);
		ColorTable colorTable;

		BYTE palBuf[0x100][3];
		palFile.file_seek(8);     				// bypass the header info
		palFile.file_read(palBuf, sizeof(palBuf));
		palFile.file_close();

		// ------- palette description -------------//

		PalDesc palBufDesc( palBuf, 3, 0x100, 8 );

		//-------- create color remap table ---------//

		colorTable.generate_table_fast( 0, palBufDesc, ColorTable::bright_func );
		memcpy( colorRemapTable, colorTable.get_table(0), sizeof(colorRemapTable) );
	}

	// ------ initialize learning campaign selection ------//
//	char *arrowBitmap = image_button.read("TRI-R");
	char *arrowBitmap = image_button.read("SCROLL-R");
	char *arrowBitmap2 = image_button.read("SCROLL-L");
	int pageUpX1 = (TLSCROLL_X2 + TLSCROLL_X1 - ((Bitmap *)arrowBitmap)->get_width()) / 2;
	int pageUpY1 = (TLSCROLL_Y2 + TLSCROLL_Y1 - ((Bitmap *)arrowBitmap)->get_height()) / 2;
	int pageUpX2 = pageUpX1 + ((Bitmap *)arrowBitmap)->get_width() - 1;
	int pageUpY2 = pageUpY1 + ((Bitmap *)arrowBitmap)->get_height() - 1;
	int pageDownX1 = (TRSCROLL_X2 + TRSCROLL_X1 - ((Bitmap *)arrowBitmap)->get_width()) / 2;
	int pageDownY1 = (TRSCROLL_Y2 + TRSCROLL_Y1 - ((Bitmap *)arrowBitmap)->get_height()) / 2;
	int pageDownX2 = pageDownX1 + ((Bitmap *)arrowBitmap)->get_width() - 1;
	int pageDownY2 = pageDownY1 + ((Bitmap *)arrowBitmap)->get_height() - 1;

	// ###### patch begin Gilbert 3/11 ######//
	int pageNoX1 = 610; // 635;
	int pageNoY1 = 87;
	int pageNoX2 = 690; // 675;
	int pageNoY2 = 107;
	// ###### patch end Gilbert 3/11 ######//

	ButtonCustomGroup learningCampaignGroup(learningCampaignCount);

	for( i = 0; i < learningCampaignGroup.button_num; ++i )
	{
		cx = 390;
		cy = ( 136 * ((learningCampaignGroup.button_num-i)*2-1) + 280*(i*2+1)) /
			(learningCampaignGroup.button_num*2);

		w = font_thin_black.text_width( learningCampaignList[i] );
		h = font_thin_black.text_height();
		learningCampaignGroup[i].create( cx-w/2-10, cy-h/2-5, cx+w/2+10, cy+h/2+5,
		i_disp_text_button, ButtonCustomPara( learningCampaignList[i], 0 ), 0 );
	}

	// -------- initialize player name field ------//

/*	GetA playerNameField;
	playerNameField.init( 364, 89, 688, tempConfig.player_name,
		tempConfig.PLAYER_NAME_LEN, &font_bold_black, 0, 1 );

		// ------ initialize human / fryhtan button ------//
	ButtonCustomGroup speciesGroup(2);
	
	w = font_thin_black.text_width( text_game_menu.str_species_humans() );
	h = font_thin_black.text_height();
	cx = 256;
	cy = 116;
	speciesGroup[0].create( cx-w/2-10, cy-h/2-5, cx+w/2+10, cy+h/2+5,
		i_disp_text_button, ButtonCustomPara( text_game_menu.str_species_humans(), 0 ), 0 );

	w = font_thin_black.text_width( text_game_menu.str_species_fryhtans() );
	h = font_thin_black.text_height();
	cx = 532;
	cy = 116;
	speciesGroup[1].create( cx-w/2-10, cy-h/2-5, cx+w/2+10, cy+h/2+5,
		i_disp_text_button, ButtonCustomPara( text_game_menu.str_species_fryhtans(), 1 ), 0 );

	// --------- initialize building size --------//
	ButtonCustomGroup buildingSizeGroup(2);
	char *buildingSizeButtonStr[2] = 
	{ 
		text_game_menu.str_building_set(1),
		text_game_menu.str_building_set(2),
	};
	cx = 440;
	cy = 386;
	for( i = 0; i < buildingSizeGroup.button_num; ++i )
	{
		w = font_thin_black.text_width(buildingSizeButtonStr[i]);
		h = font_thin_black.text_height();
		buildingSizeGroup[i].create( cx, cy, cx+w+10, cy+h+10, 
			i_disp_text_button, ButtonCustomPara(buildingSizeButtonStr[i], i+1), 0 );
		cx += w + 12;
	}
	
	// ------- loop ---------//

	{
		VgaFrontLock vgaLock;

		while(1)
		{
			MSG msg;
			if (PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE))
			{
				if (!GetMessage( &msg, NULL, 0, 0))
				{
					sys.signal_exit_flag = 1;
					// BUGHERE : vga_front is unlocked
					return 0;
				}
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				continue;
			}
			else if( sys.paused_flag || !sys.active_flag )
			{
				WaitMessage();
				continue;
			}

			if( sys.need_redraw_flag )
			{
				refreshFlag = SGOPTION_ALL;
				sys.need_redraw_flag = 0;
			}

			VgaFrontReLock vgaReLock;

			sys.yield();
			mouse.get_event();
			if( config.music_flag )
			{
				if( !music.is_playing(3) )
					music.play(3, sys.cdrom_drive ? MUSIC_CD_THEN_WAV : 0 );
			}
			else
				music.stop();


			// -------- display ----------//

			if( refreshFlag )
			{
				if( optionMode == OPTION_SPECIES )
				{
					if( refreshFlag & SGOPTION_PAGE )
					{
						vga.use_back();
						vga.disp_image_file("CHOOSE");

						// ----- display picture -------//

						const pictureWidth = 298;
						const pictureHeight = 300;

						if( !vga.use_back_buf )
						{
							image_interface.put_front( (SCROLL_SHEET_X1+SCROLL_SHEET_X2)/2-pictureWidth, 
								SCROLL_SHEET_Y2-pictureHeight, "HUMANS" );
							image_interface.put_front( (SCROLL_SHEET_X1+SCROLL_SHEET_X2)/2, 
								SCROLL_SHEET_Y2-pictureHeight, "FRYHTANS" );
						}
						else
						{
							image_interface.put_back( (SCROLL_SHEET_X1+SCROLL_SHEET_X2)/2-pictureWidth, 
								SCROLL_SHEET_Y2-pictureHeight, "HUMANS" );
							image_interface.put_back( (SCROLL_SHEET_X1+SCROLL_SHEET_X2)/2, 
								SCROLL_SHEET_Y2-pictureHeight, "FRYHTANS" );
						}

						// ----- display start, cancel button ------//

						font_thin_black.center_put( BUTTON2_X1, BUTTON2_Y1, BUTTON2_X2, BUTTON2_Y2,
							text_game_menu.str_start() );
						font_thin_black.center_put( BUTTON4_X1, BUTTON4_Y1, BUTTON4_X2, BUTTON4_Y2,
							text_game_menu.str_cancel() );

						for( i = OPTION_SPECIES; i <= OPTION_LEARNING_CAMPAIGN; ++i )
						{
							// red font for selected
							Font *fontPtr = (i == optionMode ? &font_bold_red : &font_bold_black );

							fontPtr->center_put(OPTION_SWITCH_X1, OPTION_SWITCH_Y1+OPTION_SWITCH_Y_SPACING*i,
								OPTION_SWITCH_X2, OPTION_SWITCH_Y2+OPTION_SWITCH_Y_SPACING*i, 
								m.roman_number(i+1) );
						}

						vga.use_front();
						vga.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
					}
					if( refreshFlag & SGOPTION_RACE )
						speciesGroup.paint( tempConfig.race_id < 0 );
					// other ...
				}

				if( optionMode == OPTION_LEARNING_CAMPAIGN )
				{
					if( refreshFlag & SGOPTION_PAGE )
					{
						vga.use_back();
						vga.disp_image_file("CHOOSE");

						// ------- display option Mode ------//

						for( i = OPTION_SPECIES; i <= OPTION_LEARNING_CAMPAIGN; ++i )
						{
							// red font for selected
							Font *fontPtr = (i == optionMode ? &font_bold_red : &font_bold_black );

							fontPtr->center_put(OPTION_SWITCH_X1, OPTION_SWITCH_Y1+OPTION_SWITCH_Y_SPACING*i,
								OPTION_SWITCH_X2, OPTION_SWITCH_Y2+OPTION_SWITCH_Y_SPACING*i, 
								m.roman_number(i+1) );
						}
						font_bold_black.put( 230, 386, text_game_menu.str_building_set() );
						
						// ----- display start, cancel button ------//

						font_thin_black.center_put( BUTTON2_X1, BUTTON2_Y1, BUTTON2_X2, BUTTON2_Y2,
							text_game_menu.str_start() );
						font_thin_black.center_put( BUTTON4_X1, BUTTON4_Y1, BUTTON4_X2, BUTTON4_Y2,
							text_game_menu.str_cancel() );

						vga.use_front();
						vga.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
					}

					if( refreshFlag & SGOPTION_CAMPAIGN )
						learningCampaignGroup.paint();
					if( refreshFlag & SGOPTION_BUILDING_SIZE )
						buildingSizeGroup.paint(building_size-1);
				}

				refreshFlag = 0;
			}

			sys.blt_virtual_buf();

			if( config.music_flag )
			{
				if( !music.is_playing(3) )
					music.play(3, sys.cdrom_drive ? MUSIC_CD_THEN_WAV : 0 );
			}
			else
				music.stop();

			// ------- detect --------//

			if( optionMode == OPTION_SPECIES )
			{
				if( speciesGroup.detect() >= 0 
					|| speciesGroup[speciesGroup()].detect(0,0,0,1) )	// detect pushed button, but suspend pop
				{
					if( speciesGroup() == 0 && tempConfig.race_id < 0 )
						tempConfig.race_id = 1;		// change to the first human race
					else if( speciesGroup() == 1 && tempConfig.race_id >= 0 )
						tempConfig.race_id = -1;	// change to the first monster race

					optionMode = OPTION_LEARNING_CAMPAIGN;
					refreshFlag = SGOPTION_ALL;

					// reset learningCampaignGroup state as no. of button for human and
					// monster is different
					learningCampaignGroup.button_pressed = 0;
					if (speciesGroup() == 0)
					{
						specy = 0;
						learningCampaignGroup.button_num = NO_OF_HUMAN_TUTORIAL;
					}
					else
					{
						specy = 1;
						learningCampaignGroup.button_num = NO_OF_MONSTER_TUTORIAL;
					}
				}
				else if( mouse.single_click( SCROLL_SHEET_X1, SCROLL_SHEET_Y1, (SCROLL_SHEET_X1+SCROLL_SHEET_X2)/2, SCROLL_SHEET_Y2) )
				{
					if( tempConfig.race_id < 0 )
						tempConfig.race_id = 1;		// change to the first human race
					
					optionMode = OPTION_LEARNING_CAMPAIGN;
					refreshFlag = SGOPTION_ALL;
					
					// reset learningCampaignGroup state as no. of button for human and
					// monster is different
					learningCampaignGroup.button_pressed = 0;
					learningCampaignGroup.button_num = NO_OF_HUMAN_TUTORIAL;
					specy = 0;
				}
				else if( mouse.single_click( (SCROLL_SHEET_X1+SCROLL_SHEET_X2)/2+1, SCROLL_SHEET_Y1, SCROLL_SHEET_X2, SCROLL_SHEET_Y2) )
				{
					if( tempConfig.race_id >= 0 )
						tempConfig.race_id = -1;	// change to the first monster race

					optionMode = OPTION_LEARNING_CAMPAIGN;
					refreshFlag = SGOPTION_ALL;

					// reset learningCampaignGroup state as no. of button for human and
					// monster is different
					learningCampaignGroup.button_pressed = 0;
					learningCampaignGroup.button_num = NO_OF_MONSTER_TUTORIAL;
					specy = 1;
				}
			}
			else
			if( optionMode == OPTION_LEARNING_CAMPAIGN )
			{
				if( learningCampaignGroup.detect() >= 0 
					|| learningCampaignGroup[learningCampaignGroup()].detect(0,0,0,1) )	// detect pushed button, but suspend pop
				{
				//	optionMode = OPTION_SPECIES;		// auto change to basic mode
					campaign_id = learningCampaignGroup();
					refreshFlag = SGOPTION_ALL;
				}

				else if( buildingSizeGroup.detect() >= 0)
				{
					building_size = buildingSizeGroup[buildingSizeGroup()].custom_para.value;
				}
			}

			// -------- detect switch option button ---------//

			for( i = OPTION_SPECIES; i <= OPTION_LEARNING_CAMPAIGN; ++i )
			{
				if( mouse.single_click( OPTION_SWITCH_X1, OPTION_SWITCH_Y1+OPTION_SWITCH_Y_SPACING*i,
					OPTION_SWITCH_X2, OPTION_SWITCH_Y2+OPTION_SWITCH_Y_SPACING*i) )
				{
					optionMode = i;
					refreshFlag = SGOPTION_ALL;
				}
			}

			// -------- detect start button --------//

			if( mouse.single_click( BUTTON2_X1, BUTTON2_Y1, BUTTON2_X2, BUTTON2_Y2 ) 
				&& tutorial_finish_count[specy] >= campaign_id )
			{
				// set tutorial type 
				// 0 = old tutorial which has no objective checking and use disp_guideline
				// 1 = new tutorial which has objective checking and use disp_hints
				tutorial_type = (campaign_id >= NO_OF_QUICK_START_TUTORIAL_PER_SPECY) ? 0 : 1;
	
				retFlag = campaign_id * 2 + specy + 1;
				break;			// break  while(1)
									//{ LCG()	: 0, 1, 2, 3,			
									//{ human   : 1, 3, 5, 7, ..
									//{ frythan : 2, 4, 6, 8, ..
			}

			// -------- detect cancel button --------//

			if( mouse.single_click( BUTTON4_X1, BUTTON4_Y1, BUTTON4_X2, BUTTON4_Y2 ) )
			{
				retFlag = 0;
				break;			// break  while(1)
			}

		}	// end while
	} // end of scope of VgaLock

	return retFlag;*/

	GetA playerNameField;
	playerNameField.init( 364, 89, 688, tempConfig.player_name,
		tempConfig.PLAYER_NAME_LEN, &font_bold_black, 0, 1 );

	// --------- initialize building size --------//
	ButtonCustomGroup buildingSizeGroup(2);
	char *buildingSizeButtonStr[2] = 
	{ 
		text_game_menu.str_building_set(1),
		text_game_menu.str_building_set(2),
	};
	cx = 440;
	cy = 406;
	for( i = 0; i < buildingSizeGroup.button_num; ++i )
	{
		w = font_thin_black.text_width(buildingSizeButtonStr[i]);
		h = font_thin_black.text_height();
		buildingSizeGroup[i].create( cx, cy, cx+w+10, cy+h+10, 
			i_disp_text_button, ButtonCustomPara(buildingSizeButtonStr[i], i+1), 0 );
		cx += w + 12;
	}

#ifdef DEMO
	if( tempConfig.building_size != 2 )
		tempConfig.building_size = 2;
	buildingSizeGroup[0].visible_flag = 0;		// terrain set 1 only
#endif

	
	// ------- loop ---------//
	{
		VgaFrontLock vgaLock;

		while(1)
		{
			MSG msg;
			if (PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE))
			{
				if (!GetMessage( &msg, NULL, 0, 0))
				{
					sys.signal_exit_flag = 1;
					// BUGHERE : vga_front is unlocked
					return 0;
				}
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				continue;
			}
			else if( sys.paused_flag || !sys.active_flag )
			{
				WaitMessage();
				continue;
			}

			if( sys.need_redraw_flag )
			{
				refreshFlag = SGOPTION_ALL;
				sys.need_redraw_flag = 0;
			}

			VgaFrontReLock vgaReLock;

			sys.yield();
			mouse.get_event();
			if( config.music_flag )
			{
				if( !music.is_playing(3) )
					music.play(3, sys.cdrom_drive ? MUSIC_CD_THEN_WAV : 0 );
			}
			else
				music.stop();


			// -------- display ----------//

			if( refreshFlag )
			{
				if( optionMode == OPTION_SPECIES )
				{
					if( refreshFlag & SGOPTION_PAGE )
					{
						vga.use_back();
						vga.disp_image_file("CHOOSE");

					#ifndef DEMO
						font_bold_black.put( 230, 406, text_game_menu.str_building_set() );
					#endif

						int tutorId = campaign_id_array[browseRecno-1] * 2 + specy_array[browseRecno-1] + 1;
						if( browseRecno && image_tutorial.get_index(this->operator[](tutorId)->code) )
						{
							char *bitmapPtr = image_tutorial.read(this->operator[](tutorId)->code);

							vga.active_buf->put_bitmap((VGA_WIDTH-((Bitmap *)bitmapPtr)->get_width())>>1,
								SCROLL_SHEET_Y1 +10, bitmapPtr);
							font_bold_black.center_put(0, SCROLL_SHEET_Y1 +10 +((Bitmap *)bitmapPtr)->get_height(),
								VGA_WIDTH, SCROLL_SHEET_Y1 +40 +((Bitmap *)bitmapPtr)->get_height(), this->operator[](tutorId)->des);

						//	if (!(tutorial_finish_count[specy_array[browseRecno-1]] >= campaign_id_array[browseRecno-1] ||
						//			(campaign_id_array[browseRecno-1]%2) == 0))
							if (!((tutorial_finish_count+1) >= ((browseRecno-1)>>1)))
								vga.active_buf->bar_alpha( (VGA_WIDTH-((Bitmap *)bitmapPtr)->get_width())>>1,
																	SCROLL_SHEET_Y1 +10,
																	(VGA_WIDTH+((Bitmap *)bitmapPtr)->get_width())>>1,
																	SCROLL_SHEET_Y1 +10 +((Bitmap *)bitmapPtr)->get_height(),
																	1, V_BLACK );

							int   dataSize;
							char	*text_buf = NULL;
							int	text_buf_size = 0;
							File* filePtr = res_tutor_text.get_file( tutor[tutorId]->des_text, dataSize);
							if( !filePtr )       // if error getting the tutor resource
								err_here();

							//------ Open the file and allocate buffer -----//
							FileTxt fileTxt( filePtr, dataSize );  // initialize fileTxt with an existing file stream
							if( dataSize > text_buf_size )
							{
								text_buf      = mem_resize( text_buf, dataSize );       // allocate a buffer larger than we need for the largest size possible
								text_buf_size = dataSize;
							}
							char	*textPtr = text_buf;
							fileTxt.read_paragraph(textPtr, text_buf_size);
							font_cara_w.center_put_paragraph( SCROLL_SHEET_X1+30, SCROLL_SHEET_Y1 +10 +((Bitmap *)bitmapPtr)->get_height(),
								SCROLL_SHEET_X2-30, SCROLL_SHEET_Y2, textPtr, 2, 0, 0 );

							if( text_buf )
							{
								mem_del(text_buf);
								text_buf = NULL;
							}
						}
							
						str = text_game_menu.str_training();
						font_cara_w.center_put( pageNoX1, pageNoY1, pageNoX2, pageNoY2, str );
						str = browseRecno;
						str += "/";
						str += maxTutorialRecno;
						font_cara_w.center_put( pageNoX1, pageNoY1+15, pageNoX2, pageNoY2+15, str );

						if( browseRecno > minTutorialRecno )
						//	vga.active_buf->put_bitmap_trans_decompress_hmirror( pageUpX1, pageUpY1, arrowBitmap );
							vga.active_buf->put_bitmap_trans_decompress( pageUpX1, pageUpY1, arrowBitmap2 );

						if( browseRecno < maxTutorialRecno )
							vga.active_buf->put_bitmap_trans_decompress( pageDownX1, pageDownY1, arrowBitmap );

						// ----- display start, cancel button ------//

						font_thin_black.center_put( BUTTON2_X1, BUTTON2_Y1, BUTTON2_X2, BUTTON2_Y2,
							text_game_menu.str_start() );
						font_thin_black.center_put( BUTTON4_X1, BUTTON4_Y1, BUTTON4_X2, BUTTON4_Y2,
							text_game_menu.str_cancel() );

						vga.use_front();
						vga.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
					}
				//	if( refreshFlag & SGOPTION_RACE )
				//		speciesGroup.paint( tempConfig.race_id < 0 );

				#ifndef DEMO
					if( refreshFlag & SGOPTION_BUILDING_SIZE )
						buildingSizeGroup.paint(building_size-1);
				#endif
				}

				refreshFlag = 0;
			}

			sys.blt_virtual_buf();

			if( config.music_flag )
			{
				if( !music.is_playing(3) )
					music.play(3, sys.cdrom_drive ? MUSIC_CD_THEN_WAV : 0 );
			}
			else
				music.stop();

			// ------- detect --------//

			int keyCode = mouse.is_key( mouse.scan_code, mouse.event_skey_state, (WORD) 0, K_IS_CTRL | K_CHAR_KEY );
		   if (keyCode)
			{
				keyCode = m.lower(keyCode);
				if (keyCode == 't')
				{
					tutorial_finish_count = 99;
					refreshFlag |= SGOPTION_PAGE;
					refreshFlag |= SGOPTION_BUILDING_SIZE;
				}
         }	
         
			if( optionMode == OPTION_SPECIES )
			{
				if( buildingSizeGroup.detect() >= 0)
				{
					building_size = buildingSizeGroup[buildingSizeGroup()].custom_para.value;
				}

				if( browseRecno > minTutorialRecno && mouse.any_click(TLSCROLL_X1, TLSCROLL_Y1, TLSCROLL_X2, TLSCROLL_Y2) )
				{
					// previous
					--browseRecno;
					refreshFlag |= SGOPTION_PAGE;
					refreshFlag |= SGOPTION_BUILDING_SIZE;
				}
				else if( browseRecno < maxTutorialRecno && mouse.any_click(TRSCROLL_X1, TRSCROLL_Y1, TRSCROLL_X2, TRSCROLL_Y2) )
				{
					// next
					++browseRecno;
					refreshFlag |= SGOPTION_PAGE;
					refreshFlag |= SGOPTION_BUILDING_SIZE;
				}
			}

			// -------- detect start button --------//
			//tutorial_finish_count[0] indicates which tutorial (campaign_id) of monster has been finished
			//tutorial_finish_count[1] indicates which tutorial (campaign_id) of human has been finished
			//since tutorial 1stM,1stH,3rdH,5thH can be played whenever player wants
			if( mouse.single_click( BUTTON2_X1, BUTTON2_Y1, BUTTON2_X2, BUTTON2_Y2 ) &&
				((tutorial_finish_count+1) >= ((browseRecno-1)>>1)) )
			//	(tutorial_finish_count[specy_array[browseRecno-1]] >= campaign_id_array[browseRecno-1] ||
			//	 (campaign_id_array[browseRecno-1]%2) == 0) )
			{
				// set tutorial type 
				// 0 = old tutorial which has no objective checking and use disp_guideline
				// 1 = new tutorial which has objective checking and use disp_hints
			//	tutorial_type = (campaign_id_array[browseRecno-1] >= NO_OF_QUICK_START_TUTORIAL_PER_SPECY) ? 0 : 1;
				tutorial_type = get_tutorial_type(campaign_id_array[browseRecno-1]);
	
				retFlag = campaign_id_array[browseRecno-1] * 2 + specy_array[browseRecno-1] + 1;
				break;			// break  while(1)
									//{ LCG()	: 0, 1, 2, 3,			
									//{ human   : 1, 3, 5, 7, ..
									//{ frythan : 2, 4, 6, 8, ..
			}

			// -------- detect cancel button --------//

			if( mouse.single_click( BUTTON4_X1, BUTTON4_Y1, BUTTON4_X2, BUTTON4_Y2 ) )
			{
				retFlag = 0;
				break;			// break  while(1)
			}

		}	// end while
	} // end of scope of VgaLock

	return retFlag;
}
// ------- end of function Game::select_learning_campaign_menu -----//


// ------ begin of static function i_disp_text_button ------//
//
// BustomCustom callback function to display text on top centre
//
static void i_disp_text_button(ButtonCustom *button, int repaintBody)
{
	if( !vga.use_back_buf )
		vga.blt_buf( button->x1, button->y1, button->x2, button->y2, 0 );
	Font *fontPtr = button->pushed_flag ? &font_bold_red : &font_thin_black;
	// top center align
	fontPtr->center_put( button->x1, button->y1, button->x2, button->y1+fontPtr->font_height-1,
		(char *)button->custom_para.ptr );
}
// ------ end of static function i_disp_text_button ------//


// ------ begin of static function i_disp_race_button ------//
//
// BustomCustom callback function to display race name on top centre
//
static void i_disp_race_button(ButtonCustom *button, int repaintBody)
{
	int raceId = button->custom_para.value;
	if( !vga.use_back_buf )
		vga.blt_buf( button->x1, button->y1, button->x2, button->y2, 0 );
	Font *fontPtr = button->pushed_flag ? &font_bold_red : &font_thin_black;
	// top center align
	fontPtr->center_put( button->x1, button->y1, button->x2, button->y1+fontPtr->font_height-1,
		raceId >= 0 ? race_res[raceId]->name : monster_res[-raceId]->name );
}
// ------ end of static function i_disp_race_button ------//


// ------ begin of static function i_disp_color_button ------//
//
// BustomCustom callback function to display color code marker
//
static void i_disp_color_button(ButtonCustom *button, int repaintBody)
{
	// 9,7,35,37, x+29
	int colorSchemeId = button->custom_para.value;
	short *colorRemapTable = game.color_remap_array[colorSchemeId].color_table;

	if( !button->pushed_flag )
	{
		int clipX1 = 9 + (colorSchemeId-1)*29;
		int clipY1 = 7;
		int clipX2 = clipX1 + 27 - 1;
		int clipY2 = 37;

		vga.active_buf->put_bitmap_area_trans_remap_decompress( 
			button->x1 - clipX1,	button->y1 - clipY1, image_button.read("F-COLOR"), 
			clipX1, clipY1, clipX2, clipY2,	colorRemapTable );
	}
	else
	{
		vga.active_buf->put_bitmap_trans_remap_decompress(
			button->x1, button->y1, image_button.read("F-COL-DW"), colorRemapTable );

	}
}
// ------ end of static function i_disp_color_button ------//


// ------- Begin of function Tutor::update_tutorial_finish_count -----//
//
// update the tutorial finish count in the profile
//
// no return
//
void Tutor::update_tutorial_finish_count()
{
/*	int tempTutorId = cur_tutor_id - 1;
	int isMonster = tempTutorId % 2;

#ifdef BETA
	int tutorialCount = 0;
#else
	int tutorialCount = (tempTutorId>>1) + 1;
#endif

	int tempTutorialFinishCount = player_profile.tutorial_finish_count;

	if (isMonster)
		tempTutorialFinishCount = 
		tempTutorialFinishCount %10 +
		tutorialCount * 10;
	//	player_profile.tutorial_finish_count = 
	//	player_profile.tutorial_finish_count %10 +
	//	tutorialCount * 10;
	else
		tempTutorialFinishCount = 
		(tempTutorialFinishCount /10) *10 +
		tutorialCount;
	//	player_profile.tutorial_finish_count = 
	//	(player_profile.tutorial_finish_count /10) *10 +
	//	tutorialCount;
	
	if (tempTutorialFinishCount > player_profile.tutorial_finish_count &&
			((tempTutorialFinishCount - player_profile.tutorial_finish_count) == 10 ||
			 (tempTutorialFinishCount - player_profile.tutorial_finish_count) == 1))
	{
		player_profile.tutorial_finish_count = tempTutorialFinishCount;
		player_profile.save();
	}*/

#ifndef BETA
//	{ cur_tutor_id human   : 1, 3, 5, 7, ..
//	{ cur_tutor_id frythan : 2, 4, 6, 8, ..	
	int id_array[16] = { -1,  2,  0,  3,  1,  4, -1,  5, -1,  6, -1,  7 };
	int tempTutorialFinishCount = (id_array[cur_tutor_id]>>1);
//	int tempTutorialFinishCount = 1 + (id_array[cur_tutor_id]>>1);

	if (tempTutorialFinishCount == player_profile.tutorial_finish_count + 1 &&
			(id_array[cur_tutor_id] % 2 == 0))
	{
		player_profile.tutorial_finish_count = tempTutorialFinishCount;
		player_profile.save();
	}
#endif
}
// ------- End of function Tutor::update_tutorial_finish_count -----//

// ------- Begin of function Tutor::update_tutorial_objective_finish_count -----//
//
// update the tutorial objective finish count
//
// no return
//
int Tutor::update_tutorial_objective_finish_count()
{
	if (!tutorial_type)
		return -1;

	// please notice that the objective are not forced to be finished in sequence
	for (int i = 0; i < intro_text_block_count+1; i++)
	{
		if (!achieved_objective_array[i])
		{
			switch(i)
			{
			case 0:
				if (cur_tutor_id == 3 ||
					 cur_tutor_id == 7 ||
					 cur_tutor_id == 4)
				{
					// starting of the tutorial
					// should be no checking
					// set all require condition in here
					achieved_objective_array[0] = 1;
					disable_random_event();
					set_king_not_immortal();
				//	set_fast_build();
					if (new_tutorial == 1)
						set_tech_level_max(0);
					enable_report_button_display();
					par1 = 0;
					par2 = 0;
					return 0;	
				}
				else
				if (cur_tutor_id == 9 ||
					 cur_tutor_id == 11)
				{
					// starting of the tutorial
					// should be no checking
					// set all require condition in here
					achieved_objective_array[0] = 1;
					disable_hero_joining();
					disable_message();
					disable_random_event();
					set_king_immortal();
					if (cur_tutor_id == 9)
						set_fast_build();
					if (new_tutorial == 1)
					{
						set_tech_level_max(0);
						set_tech_level_max(4, TECH_CLASS_SPYING);
						set_tech_level_max(4, TECH_CLASS_SPU_STRUCTURE); 
					}
					enable_report_button_display();
					par1 = 0;
					par2 = 0;
					return 0;	
				}
				else
				{
					// starting of the tutorial
					// should be no checking
					// set all require condition in here
					achieved_objective_array[0] = 1;
					disable_hero_joining();
					disable_message();
					disable_random_event();
					set_king_immortal();
					set_fast_build();
					if (new_tutorial == 1)
						set_tech_level_max(4);
					if (cur_tutor_id == 2)
						disable_report_button_display();
					else
						enable_report_button_display();
					par1 = 0;
					par2 = 0;
					return 0;	
				}

			case 1:
				if (cur_tutor_id == 1)
				{
					// detect continue_box clicked while the first firm selected
					if (custom_detect_is_firm_selected(game.win_condition[2].para1) &&
							continue_box_pressed == 1 &&
							achieved_objective_array[0] == 1)
					{
						achieved_objective_array[1] = 1;
						return 1;				
					}
					continue_box_pressed = 0;
				}
				else
				if (cur_tutor_id == 2)
				{	// detect seleting a town
					// cannot directly check detect_obj_recno, because
					// the selected object always not update at once after any click
					// there may be a chance of interface not showing the selected object 
					// but come to this checking subroutine
				//	if (achieved_objective_array[0] == 1 &&
				//		!town_array.is_deleted(game.win_condition[1].para1) &&
				//		detect_obj_recno == town_array[game.win_condition[1].para1]->base_obj_recno &&
				//		town_array.selected_recno == game.win_condition[1].para1)
				//	if (detect_obj_recno == allow_detect_obj_recno &&
				//			detect_press_or_release_flag == 2 &&
				//			detect_left_or_right_flag == 1 &&
				//			achieved_objective_array[0] == 1)
					if (custom_detect_is_town_selected(game.win_condition[1].para1) &&
							achieved_objective_array[0] == 1)
					{
						achieved_objective_array[1] = 1;
						return 1;				
					}
				}
				else
				if (cur_tutor_id == 3 ||
					 cur_tutor_id == 7 ||
					 cur_tutor_id == 11)
				{
					// detect continue_box clicked
					if (continue_box_pressed == 1 && achieved_objective_array[0] == 1)
					{
						achieved_objective_array[1] = 1;
						return 1;				
					}
					continue_box_pressed = 0;
				}
				else
				if (cur_tutor_id == 4)
				{
					// detect clicking on button F_RESE
					if (achieved_objective_array[0] == 1 &&
						detect_button(0,0,0,0,"F_RESE",1))
					{
						achieved_objective_array[1] = 1;
						return 1;				
					}
				}
				else
				if (cur_tutor_id == 5)
				{
					// detect continue_box clicked
					if (continue_box_pressed == 1 && achieved_objective_array[0] == 1)
					{
						achieved_objective_array[1] = 1;
						return 1;				
					}
					continue_box_pressed = 0;
				}
				else
				if (cur_tutor_id == 9)
				{
					// wait until any Tower of Science have been built
					if (nation_array.player_recno &&
							achieved_objective_array[0] == 1 &&						
							custom_search_built_firm(FIRM_RESEARCH, (~nation_array)->nation_recno))
					{
						achieved_objective_array[1] = 1;
						return 1;				
					}
				}
				break;

			case 2:
				if (cur_tutor_id == 1)
				{
					// detect clicking on button TRAIN
					if (achieved_objective_array[1] == 1 &&
							(detect_button(0,0,0,0,"TRAIN",1) || 
								(!firm_array.is_deleted(firm_array.selected_recno) &&
								 firm_array[firm_array.selected_recno]->cast_to_FirmFort()->is_soldier_full())))
					{
						achieved_objective_array[2] = 1;
						return 2;				
					}
				}
				else
				if (cur_tutor_id == 2)
				{
					// detect seleting a firm
				//	if (achieved_objective_array[1] == 1 &&
				//		!firm_array.is_deleted(game.win_condition[5].para1) &&
				//		detect_obj_recno == firm_array[game.win_condition[5].para1]->base_obj_recno &&
				//		firm_array.selected_recno == game.win_condition[5].para1)
				//	if (detect_obj_recno == allow_detect_obj_recno &&
				//			detect_press_or_release_flag == 2 &&
				//			detect_left_or_right_flag == 1 &&
				//			achieved_objective_array[1] == 1)
					if (custom_detect_is_firm_selected(game.win_condition[5].para1) &&
							achieved_objective_array[1] == 1)
					{
						achieved_objective_array[2] = 1;
						return 2;				
					}
				}
				else
				if (cur_tutor_id == 3 ||
					 cur_tutor_id == 7)
				// no more checking as this is the final goal
					break;
				else
				if (cur_tutor_id == 4)
				{
					// detect clicking on button CANCEL
					// but allow clicking anything in INFO without button area
					if (achieved_objective_array[1] == 1 &&
							detect_button(0,0,0,0,"CANCEL",1))
					{
						achieved_objective_array[2] = 1;
						return 2;				
					}					
				}
				else
				if (cur_tutor_id == 5)
				{
					// wait until any Mine of player have been built
					if (achieved_objective_array[1] == 1 &&						
							custom_search_built_firm(FIRM_MINE))
					{
						achieved_objective_array[2] = 1;
						return 2;				
					}
				}
				else
				if (cur_tutor_id == 9)
				{
					// wait until any Tower of Science have been selected
					if (achieved_objective_array[1] == 1 &&						
							custom_detect_is_firm_type_selected(FIRM_RESEARCH))
					{
						achieved_objective_array[2] = 1;
						return 2;				
					}
				}
				else
				if (cur_tutor_id == 11)
				{
					// detect continue_box clicked
					if (continue_box_pressed == 1 && achieved_objective_array[1] == 1)
					{
						achieved_objective_array[2] = 1;
						return 2;				
					}
					continue_box_pressed = 0;
				}				
				break;
				
			case 3:
				if (cur_tutor_id == 1)
				{
					// detect continue_box clicked
					if (continue_box_pressed &&
						achieved_objective_array[2] == 1)
					{
						achieved_objective_array[3] = 1;
						return 3;				
					}
				}
				else
				if (cur_tutor_id == 2)
				{
					// detect clicking on button F_PATROL
					if (achieved_objective_array[2] == 1 &&
						detect_button(0,0,0,0,"F_PATROL",1))
					{
						achieved_objective_array[3] = 1;
						return 3;				
					}
				}
				else
				if (cur_tutor_id == 4)
				{
					// wait until a magic tor is built
					// and is selected
					if (achieved_objective_array[2] == 1 &&
							custom_detect_is_firm_type_selected(FIRM_MAGIC) &&
							nation_array.player_recno &&
							custom_search_built_firm(FIRM_MAGIC))
					{
						achieved_objective_array[3] = 1;
						return 3;				
					}					
				}
				else
				if (cur_tutor_id == 5)
				{
					// wait until any Factory of player have been built
					if (achieved_objective_array[2] == 1 &&						
							custom_search_built_firm(FIRM_FACTORY))
					{
						achieved_objective_array[3] = 1;
						return 3;				
					}
				}
				else
				if (cur_tutor_id == 9)
				{
					// detect clicking on button F_PATROL
					if (achieved_objective_array[2] == 1 &&
						detect_button(0,0,0,0,"RESEARCH",1))
					{
						achieved_objective_array[3] = 1;
						return 3;				
					}
				}
				else
				if (cur_tutor_id == 11)
				{
					// detect continue_box clicked
					if (continue_box_pressed == 1 && achieved_objective_array[2] == 1)
					{
						achieved_objective_array[3] = 1;
						return 3;				
					}
					continue_box_pressed = 0;
				}				
				break;					
					
			case 4:
				if (cur_tutor_id == 1)
				{
					// detect continue_box clicked
					if (continue_box_pressed &&
						achieved_objective_array[3] == 1)
					{
						achieved_objective_array[4] = 1;
						return 4;				
					}
				}
				else
				if (cur_tutor_id == 2)
				{
					// detect clicking on right click (attack) on a town
				//	if (detect_obj_recno == allow_detect_obj_recno &&
				//			detect_press_or_release_flag == 2 &&
				//			detect_left_or_right_flag == 2 &&
				//			achieved_objective_array[3] == 1)
					if (custom_detect_is_any_unit_attacking_town(game.win_condition[1].para1) &&
						achieved_objective_array[3] == 1)
					{
						achieved_objective_array[4] = 1;
						return 4;				
					}
				}
				else
				if (cur_tutor_id == 4)
				{
					// detect continue_box clicked
					if (continue_box_pressed &&
						achieved_objective_array[3] == 1)
					{
						achieved_objective_array[4] = 1;
						return 4;				
					}
				}
				else
				if (cur_tutor_id == 5)
				{
					// wait until any Market of player have been built
					if (achieved_objective_array[3] == 1 &&						
							custom_search_built_firm(FIRM_MARKET))
					{
						achieved_objective_array[4] = 1;
						return 4;				
					}
				}
				else
				if (cur_tutor_id == 9)
				{
					// detect continue_box clicked
					if (continue_box_pressed &&
						achieved_objective_array[3] == 1)
					{
						achieved_objective_array[4] = 1;
						return 4;				
					}
				}
				else
				if (cur_tutor_id == 11)
				{
					// detect continue_box clicked
					if (continue_box_pressed == 1 && achieved_objective_array[3] == 1)
					{
						achieved_objective_array[4] = 1;
						return 4;				
					}
					continue_box_pressed = 0;
				}				
				break;

			case 5:
				if (cur_tutor_id == 1)
				{
					// detect continue_box clicked while the first firm selected 
					if (custom_detect_is_firm_selected(game.win_condition[2].para1) &&
							firm_array[game.win_condition[2].para1]->cast_to_FirmFort()->soldier_count > 0 &&
							continue_box_pressed == 1 &&
							achieved_objective_array[4] == 1)
					{
						achieved_objective_array[5] = 1;
						return 5;				
					}

					continue_box_pressed = 0;
				}
				else
				if (cur_tutor_id == 2)
				{
					// waiting for condition one - enslave an town - is true
					if( nation_array.player_recno && achieved_objective_array[4] == 1)
					{
						if (game.win_condition[1].is_condition_true((~nation_array)->nation_recno))
						{
							achieved_objective_array[5] = 1;
							return 5;				
						}
					}
				}
				else
				if (cur_tutor_id == 4)
				{
					// detect continue_box clicked
					if (continue_box_pressed &&
						achieved_objective_array[4] == 1)
					{
						achieved_objective_array[5] = 1;
						return 5;				
					}
				}
				else
				if (cur_tutor_id == 5)
				{
					// detect selecting a market
					if (achieved_objective_array[4] == 1 &&
							custom_detect_is_firm_type_selected(FIRM_MARKET))
					{
						achieved_objective_array[5] = 1;
						return 5;
					}
				}					
				else
				if (cur_tutor_id == 9)
				{
					// detect continue_box clicked
					if (continue_box_pressed &&
						achieved_objective_array[4] == 1)
					{
						achieved_objective_array[5] = 1;
						return 5;				
					}
				}
				else
				if (cur_tutor_id == 11)
				{
					// detect continue_box clicked
					if (continue_box_pressed == 1 && achieved_objective_array[4] == 1)
					{
						achieved_objective_array[5] = 1;
						return 5;				
					}
					continue_box_pressed = 0;
				}				
				break;
				
			case 6:
				if (cur_tutor_id == 1)
				{
					// detect clicking on button SORTIE
					if (achieved_objective_array[5] == 1 &&
						detect_button(0,0,0,0,"PATROL",1))
					{
						achieved_objective_array[6] = 1;
						return 6;				
					}
				}
				else
				if (cur_tutor_id == 2)
				{
					// detect clicking on button F_RETCMP
					// detect right click on a firm
					if((achieved_objective_array[5] == 1 &&
						detect_button(0,0,0,0,"F_RETCMP",1))
							||
					//	(detect_obj_recno == allow_detect_obj_recno &&
					//		detect_press_or_release_flag == 2 &&
					//		detect_left_or_right_flag == 2 &&
					//		achieved_objective_array[5] == 1))					
						(custom_detect_is_any_unit_returning_to_firm(game.win_condition[5].para1) &&
						 achieved_objective_array[5] == 1))					
					{
						achieved_objective_array[6] = 1;
						return 6;				
					}
				}
				else
				if (cur_tutor_id == 4)
				// no more checking as this is the final goal
					break;
				else
				if (cur_tutor_id == 5)
				{
					// detect continue_box clicked
					if (continue_box_pressed &&
						achieved_objective_array[5] == 1)
					{
						achieved_objective_array[6] = 1;
						return 6;				
					}
				}					
				else
				if (cur_tutor_id == 9)
				{
					// wait until any Special Structure have been built
					if (nation_array.player_recno &&
							achieved_objective_array[5] == 1 &&						
							custom_search_built_firm(FIRM_SPECIAL, (~nation_array)->nation_recno))
					{
						achieved_objective_array[6] = 1;
						return 6;				
					}
				}
				else
				if (cur_tutor_id == 11)
				{
					// detect continue_box clicked
					if (continue_box_pressed == 1 && achieved_objective_array[5] == 1)
					{
						achieved_objective_array[6] = 1;
						return 6;				
					}
					continue_box_pressed = 0;
				}				
				break;

			case 7:
				if (cur_tutor_id == 1)
				{
					// detect continue_box clicked
					if (continue_box_pressed &&
						achieved_objective_array[6] == 1)
					{
						achieved_objective_array[7] = 1;
						return 7;				
					}
				}
				else
				if (cur_tutor_id == 2)
				{
					// detect seleting the first slave town
				//	if (detect_obj_recno == allow_detect_obj_recno &&
				//			detect_press_or_release_flag == 2 &&
				//			detect_left_or_right_flag == 1 &&
				//			achieved_objective_array[6] == 1)
					if (custom_detect_is_town_selected(game.win_condition[1].para1) &&
							achieved_objective_array[6] == 1)
					{
						achieved_objective_array[7] = 1;
						return 7;				
					}
				}
				else
				if (cur_tutor_id == 5)
				{					
					// detect clicking any of your town which linked
					// to a camp of fort with an leader inside
					if (achieved_objective_array[6] == 1 && 
							nation_array.player_recno &&
							custom_detect_is_any_town_selected())
					{
						if (town_array.is_deleted(town_array.selected_recno))
							break;

						Firm* firmPtr;
						for( int i=0 ; i<town_array[town_array.selected_recno]->linked_firm_count ; i++ )
						{
							if( firm_array.is_deleted(town_array[town_array.selected_recno]->linked_firm_array[i]) )
								continue;

							firmPtr = firm_array[town_array[town_array.selected_recno]->linked_firm_array[i]];
														
							if( firmPtr->nation_recno != (~nation_array)->nation_recno	)
								continue;

							if( firmPtr->under_construction )
								continue;

							if( firmPtr->cast_to_FirmCamp() )
							{
								if (!firmPtr->cast_to_FirmCamp()->overseer_recno)
									continue;

								achieved_objective_array[7] = 1;
								return 7;				
							}		
						}
					}
				}
				else
				if (cur_tutor_id == 9)
				{
					// wait until any Special Structure have been selected
					if (achieved_objective_array[6] == 1 &&						
							custom_detect_is_firm_type_selected(FIRM_SPECIAL))
					{
						achieved_objective_array[7] = 1;
						return 7;				
					}
				}
				else
				if (cur_tutor_id == 11)
				// no more checking as this is the final goal
					break;
				break;
				
			case 8:
				if (cur_tutor_id == 1)
				{
					// detect continue_box clicked
					if (continue_box_pressed &&
						achieved_objective_array[7] == 1)
					{
						achieved_objective_array[8] = 1;
						return 8;				
					}
				}
				else
				if (cur_tutor_id == 2)
				{
					// detect seleting the lair
				//	if (detect_obj_recno == allow_detect_obj_recno &&
				//			detect_press_or_release_flag == 2 &&
				//			detect_left_or_right_flag == 1 &&
				//			achieved_objective_array[7] == 1)
					if (custom_detect_is_firm_selected(game.win_condition[5].para1) &&
							achieved_objective_array[7] == 1)
					{
						achieved_objective_array[8] = 1;
						return 8;				
					}
				}
				else
				if (cur_tutor_id == 5)
				{
					// detect continue_box clicked
					if (continue_box_pressed &&
						achieved_objective_array[7] == 1)
					{
						achieved_objective_array[8] = 1;
						return 8;				
					}
				}					
				else
				if (cur_tutor_id == 9)
				{
					// detect clicking on button S_TRAIN
					if (achieved_objective_array[7] == 1 &&
							(detect_button(0,0,0,0,"S_TRAIN",1) || 
								(!firm_array.is_deleted(firm_array.selected_recno) &&
								 firm_array[firm_array.selected_recno]->cast_to_FirmSpecial()->trainee_count >= MAX_TRAINEE )))					
					{
						achieved_objective_array[8] = 1;
						return 8;				
					}
				}
				break;
				
			case 9:
				if (cur_tutor_id == 1)
				{
					// detect continue_box clicked
					if (continue_box_pressed &&
						custom_detect_is_any_unit_selected(0, UNIT_STOP, 1, -1, 0) &&
						achieved_objective_array[8] == 1)
					{
						achieved_objective_array[9] = 1;
						return 9;				
					}

					continue_box_pressed = 0;
				}
				else
				if (cur_tutor_id == 2)
				{
					// detect clicking on button F_PATROL
					if (achieved_objective_array[8] == 1 &&
						detect_button(0,0,0,0,"F_PATROL",1))
					{
						achieved_objective_array[9] = 1;
						return 9;				
					}
				}
				else
				if (cur_tutor_id == 5)
				{
					// detect continue_box clicked
					if (continue_box_pressed &&
						achieved_objective_array[8] == 1)
					{
						achieved_objective_array[9] = 1;
						return 9;				
					}
				}
				else
				if (cur_tutor_id == 9)
				{
					// detect continue_box clicked
					if (continue_box_pressed &&
						achieved_objective_array[8] == 1)
					{
						achieved_objective_array[9] = 1;
						return 9;				
					}
				}
				break;
				
			case 10:
				if (cur_tutor_id == 1)
				{
					// detect clicking on button RETCAMP
					if (achieved_objective_array[9] == 1 &&
						detect_button(0,0,0,0,"RETCAMP",1))
					{
						achieved_objective_array[10] = 1;
						return 10;				
					}
				}
				else
				if (cur_tutor_id == 2)
				{
					// detect clicking on right click (attack) on a town
				//	if (detect_obj_recno == allow_detect_obj_recno &&
				//			detect_press_or_release_flag == 2 &&
				//			detect_left_or_right_flag == 2 &&
				//			achieved_objective_array[9] == 1)
					if (custom_detect_is_any_unit_attacking_town(game.win_condition[2].para1) &&
						achieved_objective_array[9] == 1)
					{
						achieved_objective_array[10] = 1;
						return 10;				
					}
				}
				else
				if (cur_tutor_id == 5)
				{
					// detect continue_box clicked
					if (continue_box_pressed &&
						achieved_objective_array[9] == 1)
					{
						achieved_objective_array[10] = 1;
						return 10;				
					}
				}
				else
				if (cur_tutor_id == 9)
				{
					// detect continue_box clicked
					if (continue_box_pressed &&
						achieved_objective_array[9] == 1)
					{
						achieved_objective_array[10] = 1;
						return 10;				
					}
				}
				break;
								
			case 11:
				if (cur_tutor_id == 1)
				{
					// detect continue_box clicked
					if (continue_box_pressed &&
						 achieved_objective_array[10] == 1)
					{
						achieved_objective_array[11] = 1;
						return 11;				
					}
				}
				else
				if (cur_tutor_id == 2)
				{
					// waiting for condition two - enslave an town - is true
					if( nation_array.player_recno && achieved_objective_array[10] == 1)
					{
						if (game.win_condition[2].is_condition_true((~nation_array)->nation_recno))
						{
							achieved_objective_array[11] = 1;
							return 11;				
						}
					}
				}
				if (cur_tutor_id == 5)
				{					
					// detect clicking any of your town
					if (achieved_objective_array[10] == 1 &&
						custom_detect_is_any_town_selected())
					{
						achieved_objective_array[11] = 1;
						return 11;				
					}
				}					
				else
				if (cur_tutor_id == 9)
				{
					// wait until any War Factory have been selected
					if (nation_array.player_recno &&
							achieved_objective_array[10] == 1 &&						
							custom_search_built_firm(FIRM_WAR_FACTORY, (~nation_array)->nation_recno))
					{
						achieved_objective_array[11] = 1;
						return 11;				
					}
				}
				break;
				
			case 12:
				if (cur_tutor_id == 1)
				{
					// detect selecting the town
					if (custom_detect_is_town_selected(custom_search_independent_town(0, config.race_id, 101, 0, 101, 0)) &&
							achieved_objective_array[11] == 1)
					{
						achieved_objective_array[12] = 1;
						return 12;				
					}
				}
				else
				if (cur_tutor_id == 2)
				{
					// detect clicking on button F_RETCMP
					// detect right click on a firm
					if((achieved_objective_array[11] == 1 &&
						detect_button(0,0,0,0,"F_RETCMP",1))
							||
					//	(detect_obj_recno == allow_detect_obj_recno &&
					//		detect_press_or_release_flag == 2 &&
					//		detect_left_or_right_flag == 2 &&
					//		achieved_objective_array[11] == 1))					
						(custom_detect_is_any_unit_returning_to_firm(game.win_condition[5].para1) &&
						 achieved_objective_array[11] == 1))					
					{
						achieved_objective_array[12] = 1;
						return 12;				
					}
				}
				else
				if (cur_tutor_id == 5)
				{
					// detect continue_box clicked
					if (continue_box_pressed &&
						achieved_objective_array[11] == 1)
					{
						achieved_objective_array[12] = 1;
						return 12;				
					}
				}
				else
				if (cur_tutor_id == 9)
				{
					// wait until any War Factory have been selected
					if (achieved_objective_array[11] == 1 &&						
							custom_detect_is_firm_type_selected(FIRM_WAR_FACTORY))
					{
						achieved_objective_array[12] = 1;
						return 12;				
					}
				}
				break;
				
			case 13:
				if (cur_tutor_id == 1)
				{
					// detect continue_box clicked
					if (continue_box_pressed &&
						 achieved_objective_array[12] == 1)
					{
						achieved_objective_array[13] = 1;
						return 13;				
					}
				}
				else
				if (cur_tutor_id == 2)
				{
					// detect seleting a firm
				//	if (detect_obj_recno == allow_detect_obj_recno &&
				//			detect_press_or_release_flag == 2 &&
				//			detect_left_or_right_flag == 1 &&
				//			achieved_objective_array[12] == 1)
					if (custom_detect_is_firm_selected(game.win_condition[5].para1) &&
						achieved_objective_array[12] == 1)
					{
						achieved_objective_array[13] = 1;
						return 13;				
					}
				}
				else
				if (cur_tutor_id == 5)
				{
					// detect continue_box clicked
					if (continue_box_pressed &&
						achieved_objective_array[12] == 1)
					{
						achieved_objective_array[13] = 1;
						return 13;				
					}
				}
				else
				if (cur_tutor_id == 9)
				{
					// detect clicking on button MAKEWEAP
					if (achieved_objective_array[12] == 1 &&
						detect_button(0,0,0,0,"MAKEWEAP",1))
					{
						achieved_objective_array[13] = 1;
						return 13;				
					}
				}
				break;
		
			case 14:
				if (cur_tutor_id == 1)
				{
					// detect on selecting any soldiers
					if (custom_detect_is_any_unit_selected(0, UNIT_STOP, 1, RANK_SOLDIER, 0) &&
						 achieved_objective_array[13] == 1 &&
						 unit_array[unit_array.selected_recno]->is_human())
					{
						achieved_objective_array[14] = 1;
						return 14;			
					}
				}
				else
				if (cur_tutor_id == 2)
				{
					// detect a right click on a unit icon
					// but not allow clicking the general position 
					// check if there is unit of player in the map
					if (!nation_array.player_recno || 
						achieved_objective_array[13] == 0)
						break;
					
					for( int unitRecno = unit_array.size(); unitRecno > 0; --unitRecno )
					{
						if( unit_array.is_deleted(unitRecno) )
							continue;

						Unit *unitPtr = unit_array[unitRecno];
						if( unitPtr->nation_recno != (~nation_array)->nation_recno	)
							continue;

						if( !unitPtr->is_visible() )
							continue;

						if( unitPtr->cur_order.mode != UNIT_STOP )
							continue;

						achieved_objective_array[14] = 1;
						return 14;				
					}
				}
				else
				if (cur_tutor_id == 5)
				{
					// detect on selecting any soldiers
					if (custom_detect_view_mode(MODE_NATION) &&
						 achieved_objective_array[13] == 1)
					{
						achieved_objective_array[14] = 1;
						return 14;			
					}
				}
				else
				if (cur_tutor_id == 9)
				{
					// detect continue_box clicked
					if (continue_box_pressed &&
						achieved_objective_array[13] == 1)
					{
						achieved_objective_array[14] = 1;
					#ifdef DEMO	
						achieved_objective_array[15] = 1;
						achieved_objective_array[16] = 1;
						achieved_objective_array[17] = 1;
						achieved_objective_array[18] = 1;
						achieved_objective_array[19] = 1;
						achieved_objective_array[20] = 1;
						achieved_objective_array[21] = 1;
						achieved_objective_array[22] = 1;
						return 22;
					#endif
						return 14;				
					}
				}
				break;					

			case 15:
				if (cur_tutor_id == 1)
				{
					// detect clicking on button BUILD
					if (achieved_objective_array[14] == 1 &&
						detect_button(0,0,0,0,"BUILD",1))
					{
						achieved_objective_array[15] = 1;
						return 15;				
					}
				}
				if (cur_tutor_id == 2)
				{
					// detect selecting any no cur_order unit belong to player
				//	if (detect_obj_recno > 0 &&
				//		 base_obj_array[detect_obj_recno]->cast_to_Unit() &&
				//		 nation_array.player_recno &&
				//		 ((Unit*)base_obj_array[detect_obj_recno])->nation_recno == (~nation_array)->nation_recno &&
				//		 ((Unit*)base_obj_array[detect_obj_recno])->cur_order.mode == UNIT_STOP &&
				//		 achieved_objective_array[14] == 1)
					if (achieved_objective_array[14] == 1 &&
						custom_detect_is_any_unit_selected(0, UNIT_STOP, 1, -1, 0))
					{
						achieved_objective_array[15] = 1;
						return 15;				
					}
				}
				else
				if (cur_tutor_id == 5)
				{
					// detect propose trade treaty
					if (custom_detect_talk_message_id(1) &&
						achieved_objective_array[14] == 1)
					{
						achieved_objective_array[15] = 1;
						return 15;
					}
				}
				else
				if (cur_tutor_id == 9)
				{
					// wait until any Seat of Power have been built
					if (nation_array.player_recno &&
							achieved_objective_array[14] == 1 &&						
							custom_search_built_firm(FIRM_BASE, (~nation_array)->nation_recno, 1))
					{
						achieved_objective_array[15] = 1;
						return 15;				
					}
				}
				break;
						
			case 16:
				if (cur_tutor_id == 1)
				{
					// detect continue_box clicked
					if (continue_box_pressed &&
						 achieved_objective_array[15] == 1)
					{
						achieved_objective_array[16] = 1;
						return 16;				
					}
				}
				else
				if (cur_tutor_id == 2)
				{
					// detect clicking on button F_PROMOT
					if (achieved_objective_array[15] == 1 &&
						detect_button(0,0,0,0,"F_PROMOT",1))
					{
						achieved_objective_array[16] = 1;
						return 16;				
					}
				}
				else
				if (cur_tutor_id == 5)
				{
					// detect selecting a market
					if (achieved_objective_array[15] == 1 &&
						custom_detect_is_firm_type_selected(FIRM_MARKET))
					{
						achieved_objective_array[16] = 1;
						return 16;
					}
				}					
				else
				if (cur_tutor_id == 9)
				{
					// detect selecting a Seat of Power
					if (achieved_objective_array[15] == 1 &&
						custom_detect_is_firm_type_selected(FIRM_BASE))
					{
						achieved_objective_array[16] = 1;
						return 16;
					}
				}					
				break;

			case 17:
				if (cur_tutor_id == 1)
				{
					// detect on selecting any soldiers
					if (custom_detect_is_any_unit_selected(0, UNIT_STOP, 1, RANK_SOLDIER, 0, -1, 1) &&
						 achieved_objective_array[16] == 1)
					{
						achieved_objective_array[17] = 1;
						return 17;			
					}
				}
				else
				if (cur_tutor_id == 2)
				{
					// detect seleting a firm
					if (detect_obj_recno == allow_detect_obj_recno &&
							detect_press_or_release_flag == 2 &&
							detect_left_or_right_flag == 1 &&
							achieved_objective_array[16] == 1)
					{
						achieved_objective_array[17] = 1;
						return 17;				
					}
				}
				else
				if (cur_tutor_id == 5)
				{
					// detect clicking on button HIRECARA
					if (achieved_objective_array[16] == 1 &&
						detect_button(0,0,0,0,"HIRECARA",1))
					{
						achieved_objective_array[17] = 1;
						return 17;				
					}
				}
				else
				if (cur_tutor_id == 9)
				{
					// detect clicking on button GDINVOKE
					if (achieved_objective_array[16] == 1 &&
						detect_button(0,0,0,0,"GDINVOKE",1))
					{
						achieved_objective_array[17] = 1;
						return 17;				
					}
				}
				break;
				
			case 18:
				if (cur_tutor_id == 1)
				{
					// detect clicking on button PROMOTE
					if (achieved_objective_array[17] == 1 &&
						detect_button(0,0,0,0,"PROMOTE",1))
					{
						achieved_objective_array[18] = 1;
						return 18;				
					}
				}
				else
				if (cur_tutor_id == 2)
				{
					// detect seleting a any unit belong to player
				//	if (detect_obj_recno > 0 &&
				//		 base_obj_array[detect_obj_recno]->cast_to_Unit() &&
				//		 nation_array.player_recno &&
				//		 ((Unit*)base_obj_array[detect_obj_recno])->nation_recno == (~nation_array)->nation_recno &&
				//		 ((Unit*)base_obj_array[detect_obj_recno])->cur_order.mode == UNIT_STOP &&
				//		 achieved_objective_array[17] == 1)
					if (achieved_objective_array[17] == 1 &&
						custom_detect_is_any_unit_selected(0, UNIT_STOP, 1, -1, 0))
					{
						achieved_objective_array[18] = 1;
						return 18;				
					}
				}
				else
				if (cur_tutor_id == 5)
				{
					// detect clicking on any caravan
					if (achieved_objective_array[17] == 1 &&
						custom_detect_is_unit_type_selected(UNIT_CARAVAN))
					{
						achieved_objective_array[18] = 1;
						return 18;				
					}
				}
				else
				if (cur_tutor_id == 9)
				{
					// detect clicking on any caravan
					if (achieved_objective_array[17] == 1 &&
							(custom_detect_is_unit_type_selected(UNIT_NORMAN_GOD) ||
							 custom_detect_is_unit_type_selected(UNIT_VIKING_GOD) ||
							 custom_detect_is_unit_type_selected(UNIT_CELTIC_GOD) ||
							 custom_detect_is_unit_type_selected(UNIT_ROMAN_GOD) ||
							 custom_detect_is_unit_type_selected(UNIT_GREEK_GOD) ||
							 custom_detect_is_unit_type_selected(UNIT_CARTH_GOD) ||
							 custom_detect_is_unit_type_selected(UNIT_CHINESE_GOD) ||
							 custom_detect_is_unit_type_selected(UNIT_JAPANESE_GOD) ||
							 custom_detect_is_unit_type_selected(UNIT_MONGOL_GOD) ||
							 custom_detect_is_unit_type_selected(UNIT_EGYPTIAN_GOD) ||
							 custom_detect_is_unit_type_selected(UNIT_PERSIAN_GOD) ||
							 custom_detect_is_unit_type_selected(UNIT_INDIAN_GOD)))
					{
						achieved_objective_array[18] = 1;
						return 18;				
					}
				}
				break;					
						
			case 19:
				if (cur_tutor_id == 1)
				{
					// detect continue_box clicked
					if (continue_box_pressed &&
						 achieved_objective_array[18] == 1)
					{
						achieved_objective_array[19] = 1;
						return 19;				
					}
				}
				else
				if (cur_tutor_id == 2)
				{
					// detect clicking on button BUILD
					if (achieved_objective_array[18] == 1 &&
						detect_button(0,0,0,0,"BUILD",1))
					{
						achieved_objective_array[19] = 1;
						return 19;				
					}
				}
				else
				if (cur_tutor_id == 5)
				{
					// detect continue_box clicked
					if (continue_box_pressed == 1 && achieved_objective_array[18] == 1)
					{
						achieved_objective_array[19] = 1;
						return 19;				
					}
					continue_box_pressed = 0;
				}
				else
				if (cur_tutor_id == 9)
				{
					// detect continue_box clicked
					if (continue_box_pressed &&
						 achieved_objective_array[18] == 1)
					{
						achieved_objective_array[19] = 1;
						return 19;				
					}
				}
				break;
						
			case 20:
				if (cur_tutor_id == 1)
				{
					// wait until any fort of player have been fully occupied
					if (!nation_array.player_recno || 
						achieved_objective_array[19] == 0)
						break;
					
					for( int firmRecno = firm_array.size(); firmRecno > 0; --firmRecno )
					{
						if( firm_array.is_deleted(firmRecno) )
							continue;

						Firm *firmPtr = firm_array[firmRecno];
						if( firmPtr->nation_recno != (~nation_array)->nation_recno	)
							continue;

						if( firmPtr->under_construction )
							continue;

						if( firmPtr->cast_to_FirmCamp() )
						{
							if (!firmPtr->cast_to_FirmCamp()->is_soldier_full())
								continue;

							achieved_objective_array[20] = 1;
							return 20;				
						}
					}
				}
				else
				if (cur_tutor_id == 2)
				{
					// wait until a lair linked to the independent lair is built
					if (!nation_array.player_recno || 
						achieved_objective_array[19] == 0)
						break;
					
					for( int firmRecno = firm_array.size(); firmRecno > 0; --firmRecno )
					{
						if( firm_array.is_deleted(firmRecno) )
							continue;

						Firm *firmPtr = firm_array[firmRecno];
						if( firmPtr->nation_recno != (~nation_array)->nation_recno	)
							continue;

						if( firmPtr->under_construction )
							continue;

						if( firmPtr->cast_to_FirmLair() )
						{
							for(int  t=0 ; t<firmPtr->linked_firm_count ; t++ )
							{
								if (game.win_condition[3].para1 == firmPtr->linked_firm_array[t] &&
									firmPtr->cast_to_FirmLair()->overseer_recno)
								{
									achieved_objective_array[20] = 1;
									return 20;				
								}
							}
						}
					}
				}
				else
				if (cur_tutor_id == 5)
				{
					// Find a CARAVAN of player with a foreign stop
					if (!nation_array.player_recno || 
						achieved_objective_array[19] == 0)
						break;
				
					for( int unitRecno = unit_array.size(); unitRecno > 0; --unitRecno )
					{
						if( unit_array.is_deleted(unitRecno) )
							continue;
			
						Unit *unitPtr = unit_array[unitRecno];
						if(!( unitPtr->nation_recno == (~nation_array)->nation_recno && unitPtr->unit_id == UNIT_CARAVAN ))
								continue;

						UnitCaravan* caravanPtr = (UnitCaravan*) unit_array[unitRecno];
						CaravanStop *stopPtr = caravanPtr->stop_array;
						for(int i=0; i<MAX_STOP_FOR_CARAVAN; i++, stopPtr++)
						{
							if(firm_array.is_deleted(stopPtr->firm_recno) )
								continue;

							Firm *firmPtr = firm_array[stopPtr->firm_recno];

							if( firmPtr->nation_recno != nation_array.player_recno )
							{
								achieved_objective_array[20] = 1;
								return 20;				
							}
						}
					}
				}
				else
				if (cur_tutor_id == 9)
				{
					// detect selecting a Seat of Power
					if (achieved_objective_array[19] == 1 &&
						custom_detect_is_firm_type_selected(FIRM_BASE))
					{
						achieved_objective_array[20] = 1;
						return 20;
					}
				}					
				break;					

			case 21:
				if (cur_tutor_id == 1)
				{
					// detect selecting a town
					// with indepenent, same race
					if (town_array.selected_recno &&
						!town_array.is_deleted(town_array.selected_recno) &&
						town_array[town_array.selected_recno]->nation_recno == 0 &&
						town_array[town_array.selected_recno]->race_id == config.race_id &&
						achieved_objective_array[20] == 1)
					{
						par1 = town_array.selected_recno;
						achieved_objective_array[21] = 1;
						return 21;				
					}
				}
				else
				if (cur_tutor_id == 2)
				{
					// detect seleting a firm
					if (detect_obj_recno == allow_detect_obj_recno &&
							detect_press_or_release_flag == 2 &&
							detect_left_or_right_flag == 1 &&
							achieved_objective_array[20] == 1)
					{
						achieved_objective_array[21] = 1;
						return 21;				
					}
				}
				else
				if (cur_tutor_id == 5)
				{
					// detect clicking on any caravan
					if (achieved_objective_array[20] == 1 &&
						custom_detect_is_unit_type_selected(UNIT_CARAVAN))
					{
						achieved_objective_array[21] = 1;
						return 21;				
					}
				}
				else
				if (cur_tutor_id == 9)
				{
					// detect clicking on button BLESS
					if (achieved_objective_array[20] == 1 &&
						detect_button(0,0,0,0,"BLESS",1))
					{
						achieved_objective_array[21] = 1;
						return 21;				
					}
				}
				break;
			
			case 22:
				if (cur_tutor_id == 1)
				{
					// detect continue_box clicked
					if (continue_box_pressed &&
						 achieved_objective_array[21] == 1)
					{
						achieved_objective_array[22] = 1;
						return 22;				
					}
				}
				else
				if (cur_tutor_id == 2)
				{
					// detect clicking on button BRIBE
					if (achieved_objective_array[21] == 1 &&
						detect_button(0,0,0,0,"F_GRANT",1))
					{
						achieved_objective_array[22] = 1;
						return 22;				
					}
				}
				else
				if (cur_tutor_id == 5)
				{
					// detect continue_box clicked
					if (continue_box_pressed == 1 && achieved_objective_array[21] == 1)
					{
						achieved_objective_array[22] = 1;
						return 22;				
					}
					continue_box_pressed = 0;
				}
				else
				if (cur_tutor_id == 9)
				{
					// detect continue_box clicked
					if (continue_box_pressed &&
						 achieved_objective_array[21] == 1)
					{
						achieved_objective_array[22] = 1;
						return 22;				
					}
				}
				break;
		
			case 23:
				if (cur_tutor_id == 1)
				{
					// wait until an independent town is joined
					if (!nation_array.player_recno || 
						achieved_objective_array[22] == 0)
						break;
					
					if (!town_array.is_deleted(par1) &&
						town_array[par1]->nation_recno == (~nation_array)->nation_recno)
					{		
						achieved_objective_array[23] = 1;
						return 23;				
					}
				}
				else
				if (cur_tutor_id == 2)
				{
					// waiting for condition one - enslave an town - is true
					if( nation_array.player_recno && achieved_objective_array[22] == 1)
					{
						if (game.win_condition[3].is_condition_true((~nation_array)->nation_recno))
						{
							achieved_objective_array[23] = 1;
							return 23;				
						}
					}
				}
				else
				if (cur_tutor_id == 5)
				{
					// detect continue_box clicked
					if (continue_box_pressed == 1 && achieved_objective_array[22] == 1)
					{
						achieved_objective_array[23] = 1;
						return 23;				
					}
					continue_box_pressed = 0;
				}
				else
				if (cur_tutor_id == 9)
				{
					// detect selecting an Inn
					if (achieved_objective_array[22] == 1 &&
						custom_detect_is_firm_type_selected(FIRM_INN))
					{
						achieved_objective_array[23] = 1;
						return 23;
					}
				}					
				break;
				
			case 24:
				if (cur_tutor_id == 1)
				{
					// waiting for condition three - enslave an town - is true
					if( nation_array.player_recno )
					{
						if (game.win_condition[5].is_condition_true((~nation_array)->nation_recno))
						{
							achieved_objective_array[24] = 1;
							return 24;				
						}
					}
				}
				else
				if (cur_tutor_id == 2)
				{
					// waiting for condition one - enslave an town - is true
					if( nation_array.player_recno && achieved_objective_array[23] == 1)
					{
						if (game.win_condition[4].is_condition_true((~nation_array)->nation_recno))
						{
							achieved_objective_array[24] = 1;
							return 24;				
						}
					}
				}
				else
				if (cur_tutor_id == 5)
				{
					// detect continue_box clicked
					if (continue_box_pressed == 1 && achieved_objective_array[23] == 1)
					{
						achieved_objective_array[24] = 1;
						return 24;				
					}
					continue_box_pressed = 0;
				}
				else
				if (cur_tutor_id == 9)
				{
					// detect continue_box clicked
					if (continue_box_pressed == 1 && achieved_objective_array[23] == 1)
					{
						achieved_objective_array[24] = 1;
						return 24;				
					}
					continue_box_pressed = 0;
				}
				break;					

			case 25:
				if (cur_tutor_id == 1)
				{
					// waiting for condition three - enslave an town - is true
					if( nation_array.player_recno )
					{
						if (game.win_condition[6].is_condition_true((~nation_array)->nation_recno))
						{
							achieved_objective_array[25] = 1;
							return 25;				
						}
					}
				}
				else
				if (cur_tutor_id == 5)
				{
					// detect clicking on any caravan
					if (achieved_objective_array[24] == 1 &&
						custom_detect_is_unit_type_selected(UNIT_CARAVAN))
					{
						achieved_objective_array[25] = 1;
						return 25;				
					}
				}
				else
				if (cur_tutor_id == 9)
				{
					// detect continue_box clicked
					if (continue_box_pressed == 1 && achieved_objective_array[24] == 1)
					{
						achieved_objective_array[25] = 1;
						return 25;				
					}
					continue_box_pressed = 0;
				}
				break;					
				
			case 26:
				if (cur_tutor_id == 5)
				{
					// detect clicking on button CARACOPY
					if (achieved_objective_array[25] == 1 &&
						detect_button(0,0,0,0,"CARACOPY",1))
					{
						achieved_objective_array[26] = 1;
						return 26;				
					}
				}
				else
				if (cur_tutor_id == 9)
				{
					// detect selecting an Inn
					if (achieved_objective_array[25] == 1 &&
						custom_detect_is_firm_type_selected(FIRM_INN))
					{
						achieved_objective_array[26] = 1;
						return 26;
					}
				}					
				break;					

			case 27:
				if (cur_tutor_id == 5)
				{
					// detect continue_box clicked
					if (continue_box_pressed == 1 && achieved_objective_array[26] == 1)
					{
						achieved_objective_array[27] = 1;
						return 27;				
					}
					continue_box_pressed = 0;
				}
				else
				if (cur_tutor_id == 9)
				{
					// detect continue_box clicked
					if (continue_box_pressed == 1 && achieved_objective_array[26] == 1)
					{
						achieved_objective_array[27] = 1;
						return 27;				
					}
					continue_box_pressed = 0;
				}
				break;
				
			case 28:
				if (cur_tutor_id == 5)
				// no more checking as this is the final goal
					break;
				else
				if (cur_tutor_id == 9)
				{
					// detect continue_box clicked
					if (continue_box_pressed == 1 && achieved_objective_array[27] == 1)
					{
						achieved_objective_array[28] = 1;
						return 28;				
					}
					continue_box_pressed = 0;
				}
				break;

			case 29:
				if (cur_tutor_id == 9)
				{
				// detect selecting an Spy College
					if (achieved_objective_array[28] == 1 &&
						custom_detect_is_firm_type_selected(FIRM_SPY))
					{
						achieved_objective_array[29] = 1;
						return 29;
					}
				}
				break;

			case 30:
				if (cur_tutor_id == 9)
				{
					// detect clicking on button TRAIN-C or TRAIN-I or TRAIN-S
					if (achieved_objective_array[29] == 1 &&
							(detect_button(0,0,0,0,"TRAIN-C",1) ||
							 detect_button(0,0,0,0,"TRAIN-I",1) ||
							 detect_button(0,0,0,0,"TRAIN-S",1) ||
								(!firm_array.is_deleted(firm_array.selected_recno) &&
								 firm_array[firm_array.selected_recno]->cast_to_FirmSpy()->trainee_count >= MAX_TRAINEE)))
					 {
						achieved_objective_array[30] = 1;
						return 30;				
					}
				}
				break;

			case 31:
				if (cur_tutor_id == 9)
				{					
					// detect clicking any of your town which 
					// has an own spy inside
					if (achieved_objective_array[30] == 1 && 
							nation_array.player_recno &&
							custom_detect_is_any_town_selected())
					{
						if (town_array.is_deleted(town_array.selected_recno))
							break;

						Spy* spyPtr;
						for( int i=spy_array.size() ; i>=1 ; i-- )
						{
							if( spy_array.is_deleted(i) )
								continue;

							spyPtr = spy_array[i];

							if( spyPtr->spy_place == SPY_TOWN &&
								 spyPtr->spy_place_para == town_array.selected_recno &&
								 spyPtr->true_nation_recno == (~nation_array)->nation_recno )
							{
								achieved_objective_array[31] = 1;
								return 31;				
							}
						}
					}
				}
				break;

			case 32:
				if (cur_tutor_id == 9)
				{
					// detect continue_box clicked
					if (continue_box_pressed == 1 && achieved_objective_array[31] == 1)
					{
						achieved_objective_array[32] = 1;
						return 32;				
					}
					continue_box_pressed = 0;
				}
				break;

			case 33:
				if (cur_tutor_id == 9)
				{
					// detect clicking on any spy
					if (achieved_objective_array[32] == 1 &&
							custom_detect_is_any_unit_selected(0, UNIT_STOP, 1, -1, -1, 1, 1))
					{
						achieved_objective_array[33] = 1;
						return 33;				
					}
				}
				break;					
			
			case 34:
				if (cur_tutor_id == 9)
				{
					// detect continue_box clicked
					if (continue_box_pressed == 1 && 
							achieved_objective_array[33] == 1 &&
							nation_array.player_recno &&
							!unit_array.is_deleted(unit_array.selected_recno) &&
							unit_array[unit_array.selected_recno]->nation_recno == (~nation_array)->nation_recno)
					{
						achieved_objective_array[34] = 1;
						return 34;				
					}
					continue_box_pressed = 0;
				}
				break;

			case 35:
				if (cur_tutor_id == 9)
				{
					// detect clicking on button SPYNOTI0
					if (achieved_objective_array[34] == 1 &&
							detect_button(0,0,0,0,"SPYNOTI0",1))
					{
						achieved_objective_array[35] = 1;
						return 35;				
					}
				}
				break;

			case 36:
				if (cur_tutor_id == 9)
				{
					// detect clicking on button SPYNOTI1
					if (achieved_objective_array[35] == 1 &&
							detect_button(0,0,0,0,"SPYNOTI1",1))
					{
						achieved_objective_array[36] = 1;
						return 36;				
					}
				}
				break;

			case 37:
				if (cur_tutor_id == 9)
				{
					// detect continue_box clicked
					if (continue_box_pressed == 1 && achieved_objective_array[36] == 1)
					{
						achieved_objective_array[37] = 1;
						return 37;				
					}
					continue_box_pressed = 0;
				}
				break;

			case 38:
				if (cur_tutor_id == 9)
				{					
					// detect clicking any of enemy town which 
					// has an own spy inside
					if (achieved_objective_array[37] == 1 && 
						nation_array.player_recno &&
						!town_array.is_deleted(town_array.selected_recno) &&
						town_array[town_array.selected_recno]->nation_recno != 
							(~nation_array)->nation_recno )
					{
						Spy* spyPtr;
						for( int i=spy_array.size() ; i>=1 ; i-- )
						{
							if( spy_array.is_deleted(i) )
								continue;

							spyPtr = spy_array[i];

							if( spyPtr->spy_place == SPY_TOWN &&
								 spyPtr->spy_place_para == town_array.selected_recno &&
								 spyPtr->true_nation_recno == (~nation_array)->nation_recno )
							{
								achieved_objective_array[38] = 1;
								return 38;				
							}
						}
					}
				}
				break;

			case 39:
				if (cur_tutor_id == 9)
				{
					// detect clicking on button SPYMENU
					if (achieved_objective_array[38] == 1 &&
							detect_button(0,0,0,0,"SPYMENU",1))
					{
						achieved_objective_array[39] = 1;
						return 39;				
					}
				}
				break;

			case 40:
				if (cur_tutor_id == 9)
				{
					// detect clicking on button CHG-MISS
					if (achieved_objective_array[39] == 1 &&
							detect_button(0,0,0,0,"CHG-MISS",1))
					{
						achieved_objective_array[40] = 1;
						return 40;				
					}
				}
				break;

			case 41:
				if (cur_tutor_id == 9)
				{					
					// detect clicking any of enemy firm which 
					// has an own spy inside
					if (achieved_objective_array[40] == 1 && 
						nation_array.player_recno &&
						!firm_array.is_deleted(firm_array.selected_recno) &&
						firm_array[firm_array.selected_recno]->nation_recno != 
							(~nation_array)->nation_recno &&
						firm_array[firm_array.selected_recno]->cast_to_FirmCamp())
					{
						Spy* spyPtr;
						for( int i=spy_array.size() ; i>=1 ; i-- )
						{
							if( spy_array.is_deleted(i) )
								continue;

							spyPtr = spy_array[i];

							if( spyPtr->spy_place == SPY_FIRM &&
								 spyPtr->spy_place_para == firm_array.selected_recno &&
								 spyPtr->true_nation_recno == (~nation_array)->nation_recno )
							{
								achieved_objective_array[41] = 1;
								return 41;				
							}
						}
					}
				}
				break;

			case 42:
				if (cur_tutor_id == 9)
				{
					// detect continue_box clicked
					if (continue_box_pressed == 1 && achieved_objective_array[41] == 1)
					{
						achieved_objective_array[42] = 1;
						return 42;				
					}
					continue_box_pressed = 0;
				}
				break;
			
			case 43:
				if (cur_tutor_id == 9)
				{
					// detect clicking any of enemy firm which 
					// has an own spy inside &&
					// detect continue_box clicked
					if (achieved_objective_array[42] == 1 && 
						continue_box_pressed == 1 &&
						nation_array.player_recno &&
						!firm_array.is_deleted(firm_array.selected_recno) &&
						firm_array[firm_array.selected_recno]->nation_recno != 
							(~nation_array)->nation_recno &&
						firm_array[firm_array.selected_recno]->cast_to_FirmCamp())
					{
						Spy* spyPtr;
						for( int i=spy_array.size() ; i>=1 ; i-- )
						{
							if( spy_array.is_deleted(i) )
								continue;

							spyPtr = spy_array[i];

							if( spyPtr->spy_place == SPY_FIRM &&
								 spyPtr->spy_place_para == firm_array.selected_recno &&
								 spyPtr->true_nation_recno == (~nation_array)->nation_recno )
							{
								achieved_objective_array[43] = 1;
								return 43;				
							}
						}
					}
					continue_box_pressed = 0;
				}
				break;

			case 44:
				if (cur_tutor_id == 9)
				{
					// detect continue_box clicked
					if (continue_box_pressed == 1 && achieved_objective_array[43] == 1)
					{
						achieved_objective_array[44] = 1;
						return 44;				
					}
					continue_box_pressed = 0;
				}
				break;

			case 45:
				if (cur_tutor_id == 9)
				{
					// detect continue_box clicked
					if (continue_box_pressed == 1 && achieved_objective_array[44] == 1)
					{
						achieved_objective_array[45] = 1;
						return 45;				
					}
					continue_box_pressed = 0;
				}
				break;

			case 46:
				if (cur_tutor_id == 9)
				{
					// detect continue_box clicked
					if (continue_box_pressed == 1 && achieved_objective_array[45] == 1)
					{
						achieved_objective_array[46] = 1;
						return 46;				
					}
					continue_box_pressed = 0;
				}
				break;

			case 47:
				if (cur_tutor_id == 9)
				{					
					// detect clicking any of enemy firm which 
					// has an own spy inside
					if (achieved_objective_array[46] == 1 && 
						nation_array.player_recno &&
						!firm_array.is_deleted(firm_array.selected_recno) &&
						firm_array[firm_array.selected_recno]->nation_recno != 
							(~nation_array)->nation_recno &&
						firm_array[firm_array.selected_recno]->cast_to_FirmCamp())
					{
						Spy* spyPtr;
						for( int i=spy_array.size() ; i>=1 ; i-- )
						{
							if( spy_array.is_deleted(i) )
								continue;

							spyPtr = spy_array[i];

							if( spyPtr->spy_place == SPY_FIRM &&
								 spyPtr->spy_place_para == firm_array.selected_recno &&
								 spyPtr->true_nation_recno == (~nation_array)->nation_recno )
							{
								achieved_objective_array[47] = 1;
								return 47;				
							}
						}
					}
				}
				break;

			case 48:
				if (cur_tutor_id == 9)
				{
					// detect clicking on button SPYMENU
					if (achieved_objective_array[47] == 1 &&
							detect_button(0,0,0,0,"SPYMENU",1))
					{
						achieved_objective_array[48] = 1;
						return 48;				
					}
				}
				break;

			case 49:
				if (cur_tutor_id == 9)
				{
					// detect continue_box clicked
					if (continue_box_pressed == 1 && achieved_objective_array[48] == 1)
					{
						achieved_objective_array[49] = 1;
						return 49;				
					}
					continue_box_pressed = 0;
				}
				break;

			case 50:
				if (cur_tutor_id == 9)
				{
					// detect continue_box clicked
					if (continue_box_pressed == 1 && achieved_objective_array[49] == 1)
					{
						achieved_objective_array[50] = 1;
						return 50;				
					}
					continue_box_pressed = 0;
				}
				break;

			case 51:
				if (cur_tutor_id == 9)
				{
					// detect continue_box clicked
					if (continue_box_pressed == 1 && achieved_objective_array[50] == 1)
					{
						achieved_objective_array[51] = 1;
						return 51;				
					}
					continue_box_pressed = 0;
				}
				break;

			case 52:
				if (cur_tutor_id == 9)
				{
					// detect continue_box clicked
					if (continue_box_pressed == 1 && achieved_objective_array[51] == 1)
					{
						achieved_objective_array[52] = 1;
						return 52;				
					}
					continue_box_pressed = 0;
				}
				break;

			case 53:
				if (cur_tutor_id == 9)
				{
					// detect continue_box clicked
					if (continue_box_pressed == 1 && achieved_objective_array[52] == 1)
					{
						achieved_objective_array[53] = 1;
						return 53;				
					}
					continue_box_pressed = 0;
				}
				break;

			case 54:
				if (cur_tutor_id == 9)
				// no more checking as this is the final goal
					break;
						
			default:
				if( nation_array.player_recno )
				{
					if (game.win_condition[i].is_condition_true((~nation_array)->nation_recno))
					{
						achieved_objective_array[i] = 1;
						return i;				
					}
				}
			}
		}
	}
	return -1;
}
// ------- End of function Tutor::update_tutorial_objective_finish_count -----//


// ------- Begin of function Tutor::disp_objective_text -----//
void Tutor::disp_objective_text(int objestiveRecNo)
{
	if (objestiveRecNo > 0)
		play_speech(cur_tutor_id, objestiveRecNo);

	switch (objestiveRecNo)
	{		
	case 0:
		// should never be called
		// as update_tutorial_objective_finish_count() is called first and 
		// set achieved_objective_array[0] to 1
		err_here();
		break;
	
	case 1:
		if (cur_tutor_id == 1)
		{	
			// allow clicking and doing anything
			// detect seleting a firm and
			// click the continue box
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 5;
			continue_box = 1;
			
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
			load_in_game_box(cur_tutor_id, 1);
		//	strcpy( tutor_box[0].text, "This is your first Town. Click on it to see the population and loyalty level." );
			tutor_box[0].base_object_recno_no = town_array[game.win_condition[1].para1]->base_obj_recno;
		//	strcpy( tutor_box[1].text, "This is your first Fort. Click on it to go next section." );
			tutor_box[1].base_object_recno_no = firm_array[game.win_condition[2].para1]->base_obj_recno;
		}
		else
		if (cur_tutor_id == 2)
		{	
			// not allow clicking and doing anything
			// detect seleting a town
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			// set this to enable the senario editor
		//	selectively_detect_flag = 0;
			selectively_detect_flag = 1;
			allow_detect_obj_recno = town_array[game.win_condition[1].para1]->base_obj_recno;
			allow_left_or_right_click = 1;
			allow_detect_button_rc = 1;
			
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}

			load_in_game_box(cur_tutor_id, 1);
		//	strcpy( tutor_box[0].text, "This is an Independent Human Town. Click on it." );
			tutor_box[0].base_object_recno_no = allow_detect_obj_recno;
		//	strcpy( tutor_box[1].text, "This is your Lair" );
			tutor_box[1].base_object_recno_no = firm_array[game.win_condition[5].para1]->base_obj_recno;
		}
		else
		if (cur_tutor_id == 3 ||
			 cur_tutor_id == 7 ||
			 cur_tutor_id == 11)
		{	
		// allow clicking and doing anything
			// just click the continue box
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			// set this to enable the senario editor
		//	selectively_detect_flag = 0;
			selectively_detect_flag = 5;
			continue_box = 1;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		else
		if (cur_tutor_id == 4)
		{	
			// not allow clicking and doing anything
			// detect clicking on button F_RESE
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 3;
			allow_left_or_right_click = 1;
			allow_detect_button_rc = 1;
			strcpy(allow_detect_button_name, "F_RESE");

			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		
			load_in_game_box(cur_tutor_id, 1);
		//	strcpy( tutor_box[0].text, "This is the Research Icon." );
			tutor_box[0].screen_loc_centre_x = INFO_X1 + 200;
			tutor_box[0].screen_loc_centre_y = INFO_Y1 + 300;
		}
		else
		if (cur_tutor_id == 5)
		{	
		// allow clicking and doing anything
			// just click the continue box
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			// set this to enable the senario editor
		//	selectively_detect_flag = 0;
			selectively_detect_flag = 5;
			continue_box = 1;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		else
		if (cur_tutor_id == 9)
		{	
		// allow clicking and doing anything
		// wait for the finish of Tower of Science
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 5;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		break;
		           
	case 2:
		if (cur_tutor_id == 1)
		{	
			// not allow clicking and doing anything
			// detect clicking on button TRAIN
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 3;
			allow_left_or_right_click = 1;
			allow_detect_button_rc = 1;
			strcpy(allow_detect_button_name, "TRAIN");

			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		
			load_in_game_box(cur_tutor_id, 2);
		//	strcpy( tutor_box[0].text, "This is the Train Icon." );
			tutor_box[0].screen_loc_centre_x = INFO_X1 + 40;
			tutor_box[0].screen_loc_centre_y = INFO_Y1 + 300;
		}
		else
		if (cur_tutor_id == 2)
		{	
			// not allow clicking and doing anything
			// detect seleting a firm
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 1;
			allow_detect_obj_recno = firm_array[game.win_condition[5].para1]->base_obj_recno;
			allow_left_or_right_click = 1;
			allow_detect_button_rc = 1;

			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}

			load_in_game_box(cur_tutor_id, 2);
		//	strcpy( tutor_box[0].text, "This shows the Town's Resistance to your rule." );
			tutor_box[0].screen_loc_centre_x = INFO_X1 + 145;
			tutor_box[0].screen_loc_centre_y = INFO_Y1 + 180;
		//	strcpy( tutor_box[1].text, "This is your Lair. Click it." );
			tutor_box[1].base_object_recno_no = firm_array[game.win_condition[5].para1]->base_obj_recno;
		}
		else
		if (cur_tutor_id == 3 ||
			 cur_tutor_id == 7)
		{	
			// allow clicking and doing anything
			// detect all for final battle
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 0;
			tutor.update_tutorial_finish_count();
			close_box = 1;
					// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		else
		if (cur_tutor_id == 4)
		{	
			// detect clicking on button CANCEL
			// but allow clicking anything in INFO without button area
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 2;
			allow_detect_button_x1 = INFO_X1 +12 + 3*BUTTON_DISTANCE;
			allow_detect_button_y1 = INFO_Y1 +280;
			allow_detect_button_x2 = INFO_X1 +15 + 4*BUTTON_DISTANCE;
			allow_detect_button_y2 = INFO_Y2;
		
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
			load_in_game_box(cur_tutor_id, 2);
		//	strcpy( tutor_box[0].text, "These are your choices for Fryhtan Technology. These choices will be different for every species of Fryhtan." );
			tutor_box[0].screen_loc_centre_x = INFO_X1 + 160;
			tutor_box[0].screen_loc_centre_y = INFO_Y1 + 50;
			tutor_box[0].pointer_offset_x = 0;
			tutor_box[0].pointer_offset_y = -20;			
		}
		else
		if (cur_tutor_id == 5)
		{	
		// allow clicking and doing anything
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			// set this to enable the senario editor
		//	selectively_detect_flag = 0;
			selectively_detect_flag = 5;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		else
		if (cur_tutor_id == 9)
		{	
		// allow clicking and doing anything
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 5;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		else
		if (cur_tutor_id == 11)
		{	
		// allow clicking and doing anything
			// just click the continue box
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			// set this to enable the senario editor
		//	selectively_detect_flag = 0;
			selectively_detect_flag = 5;
			continue_box = 1;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		break;
		
	case 3:
		if (cur_tutor_id == 1)
		{	
			// allow clicking and doing anything
			// just click the continue box
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			// set this to enable the senario editor
		//	selectively_detect_flag = 0;
			selectively_detect_flag = 5;
			continue_box = 1;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		else
		if (cur_tutor_id == 2)
		{	
			// not allow clicking and doing anything
			// detect clicking on button F_PATROL
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 3;
			allow_left_or_right_click = 1;
			allow_detect_button_rc = 1;
			strcpy(allow_detect_button_name, "F_PATROL");

			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		
			load_in_game_box(cur_tutor_id, 3);
		//	strcpy( tutor_box[0].text, "This is the Sortie Icon." );
			tutor_box[0].screen_loc_centre_x = INFO_X1 + 90;
			tutor_box[0].screen_loc_centre_y = INFO_Y1 + 300;
		}
		else
		if (cur_tutor_id == 4)
		{	
		// allow clicking and doing anything
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			// set this to enable the senario editor
		//	selectively_detect_flag = 0;
			selectively_detect_flag = 5;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		else
		if (cur_tutor_id == 5)
		{	
		// allow clicking and doing anything
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			// set this to enable the senario editor
		//	selectively_detect_flag = 0;
			selectively_detect_flag = 5;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		else
		if (cur_tutor_id == 9)
		{	
			// not allow clicking and doing anything
			// detect clicking on button RESEARCH
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 3;
			allow_left_or_right_click = 1;
			allow_detect_button_rc = 1;
			strcpy(allow_detect_button_name, "RESEARCH");

			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		
			load_in_game_box(cur_tutor_id, 3);
		//	strcpy( tutor_box[0].text, "This is the Research Icon." );
			tutor_box[0].screen_loc_centre_x = INFO_X1 + 40;
			tutor_box[0].screen_loc_centre_y = INFO_Y1 + 300;
		}
		else
		if (cur_tutor_id == 11)
		{	
		// allow clicking and doing anything
			// just click the continue box
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			// set this to enable the senario editor
		//	selectively_detect_flag = 0;
			selectively_detect_flag = 5;
			continue_box = 1;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		break;
			
	case 4:
		if (cur_tutor_id == 1)
		{
			// allow clicking and doing anything
			// just click the continue box
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 5;
			continue_box = 1;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}

			load_in_game_box(cur_tutor_id, 4);
		//	strcpy( tutor_box[0].text, "This shows your Reputation." );
			tutor_box[0].screen_loc_centre_x = REPU_BUTTON_X1+20;
			tutor_box[0].screen_loc_centre_y = REPU_BUTTON_Y1+20;
			if (current_display_mode.mode_id == MODE_ID_1024x768x16)
			{
				tutor_box[0].pointer_offset_x = -257;
				tutor_box[0].pointer_offset_y = 110;
			}
			else
			{
				tutor_box[0].pointer_offset_x = 0;
				tutor_box[0].pointer_offset_y = 100;
			}
			
		//	strcpy( tutor_box[1].text, "This shows your Money." );
			tutor_box[1].screen_loc_centre_x = CASH_X1+20;
			tutor_box[1].screen_loc_centre_y = CASH_Y1+10;
			if (current_display_mode.mode_id == MODE_ID_1024x768x16)
			{
				tutor_box[1].pointer_offset_x = -300;
				tutor_box[1].pointer_offset_y = 60;
			}
			else
			{
				tutor_box[1].pointer_offset_x = -30;
				tutor_box[1].pointer_offset_y = 150;
			}
			
		//	strcpy( tutor_box[2].text, "This shows your Food." );
			tutor_box[2].screen_loc_centre_x = FOOD_X1+20;
			tutor_box[2].screen_loc_centre_y = FOOD_Y1+10;
			if (current_display_mode.mode_id == MODE_ID_1024x768x16)
			{
				tutor_box[2].pointer_offset_x = -300;
				tutor_box[2].pointer_offset_y = 0;
			}
			else
			{
				tutor_box[2].pointer_offset_x = -100;
				tutor_box[2].pointer_offset_y = 100;
			}
		}
		else
		if (cur_tutor_id == 2)
		{
			// not allow clicking and doing anything
			// detect clicking on right click (attack) on a town
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 1;
			allow_left_or_right_click = 2;
			allow_detect_button_rc = 2;
			allow_detect_obj_recno = town_array[game.win_condition[1].para1]->base_obj_recno;

			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}

			load_in_game_box(cur_tutor_id, 4);
		//	strcpy( tutor_box[0].text, "This is an Independent Human Town. Right click to attack it." );
			tutor_box[0].base_object_recno_no = allow_detect_obj_recno;
		}
		else
		if (cur_tutor_id == 4)
		{	
			// not allow clicking and doing anything
			// detect continue button
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 6;
			continue_box = 1;
			
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}

			load_in_game_box(cur_tutor_id, 4);
		//	strcpy( tutor_box[0].text, "This is your Power Bar. When it is blue, your Mage Tor has enough power to project its power." );
			tutor_box[0].screen_loc_centre_x = INFO_X1 + 160;
			tutor_box[0].screen_loc_centre_y = INFO_Y1 + 20;
			tutor_box[0].pointer_offset_x = 0;
			tutor_box[0].pointer_offset_y = -20;
		//	strcpy( tutor_box[1].text, "These are your options for using your Mage Tor. Click on one and then Right-Click on your target." );
			tutor_box[1].screen_loc_centre_x = INFO_X1 + 80;
			tutor_box[1].screen_loc_centre_y = INFO_Y2 - 200;
			tutor_box[1].pointer_offset_x = 0;
			tutor_box[1].pointer_offset_y = 320;			
		}
		else
		if (cur_tutor_id == 5)
		{	
		// allow clicking and doing anything
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			// set this to enable the senario editor
		//	selectively_detect_flag = 0;
			selectively_detect_flag = 5;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		else
		if (cur_tutor_id == 9)
		{
			// allow clicking and doing anything
			// just click the continue box
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 5;
			continue_box = 1;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		else
		if (cur_tutor_id == 11)
		{	
		// allow clicking and doing anything
			// just click the continue box
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			// set this to enable the senario editor
		//	selectively_detect_flag = 0;
			selectively_detect_flag = 5;
			continue_box = 1;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		break;
	
	case 5:
		if (cur_tutor_id == 1)
		{	
			// allow clicking and doing anything
			// detect seleting a firm and
			// click the continue box
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 5;
			continue_box = 1;
			
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
			load_in_game_box(cur_tutor_id, 5);
		//	strcpy( tutor_box[0].text, "This is your first Fort. Click on it to go next section." );
			tutor_box[0].base_object_recno_no = firm_array[game.win_condition[2].para1]->base_obj_recno;
		}
		else
		if (cur_tutor_id == 2)
		{	
			// not allow clicking and doing anything
			// waiting for condition one - enslave an town - is true
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 6;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
			load_in_game_box(cur_tutor_id, 5);
		//	strcpy( tutor_box[0].text, "Your troop is now enslaving this Town." );
			tutor_box[0].base_object_recno_no = town_array[game.win_condition[1].para1]->base_obj_recno;

		}
		else
		if (cur_tutor_id == 4)
		{
			// allow clicking and doing anything
			// just click the continue box
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 5;
			continue_box = 1;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		else
		if (cur_tutor_id == 5)
		{	
		// allow clicking and doing anything
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			// set this to enable the senario editor
		//	selectively_detect_flag = 0;
			selectively_detect_flag = 5;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		else
		if (cur_tutor_id == 9)
		{
			// allow clicking and doing anything
			// just click the continue box
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 5;
			continue_box = 1;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		else
		if (cur_tutor_id == 11)
		{	
		// allow clicking and doing anything
			// just click the continue box
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			// set this to enable the senario editor
		//	selectively_detect_flag = 0;
			selectively_detect_flag = 5;
			continue_box = 1;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		break;
		
	case 6:
		if (cur_tutor_id == 1)
		{	
			// not allow clicking and doing anything
			// allow any click inside the INFO area, allow and detect only SORTIE button
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 2;
			allow_unit_info_detect_rc = 1;
			allow_detect_button_rc = 1;
			strcpy(allow_detect_button_name, "PATROL");
													
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
			load_in_game_box(cur_tutor_id, 6);
		//	strcpy( tutor_box[0].text, "Click here to view the Leadership Level of all those in this Fort." );
			tutor_box[0].screen_loc_centre_x = INFO_X1 + 150;
			tutor_box[0].screen_loc_centre_y = INFO_Y1 + 210;
			tutor_box[0].pointer_offset_y = -200;
			
		//	strcpy( tutor_box[1].text, "This is the Sortie Icon." );
			tutor_box[1].screen_loc_centre_x = INFO_X1 + 90;
			tutor_box[1].screen_loc_centre_y = INFO_Y1 + 300;
		}
		else
		if (cur_tutor_id == 2)
		{	
			// not allow clicking and doing anything
			// detect clicking on button F_RETCMP
			// detect right click on a firm
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 4;
			allow_detect_obj_recno = firm_array[game.win_condition[5].para1]->base_obj_recno;
			allow_left_or_right_click = 2;
			allow_detect_button_rc = 1;
			strcpy(allow_detect_button_name, "F_RETCMP");

			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}

			load_in_game_box(cur_tutor_id, 6);
		//	strcpy( tutor_box[0].text, "This is the Return Icon." );
			tutor_box[0].screen_loc_centre_x = INFO_X1 + 150;
			tutor_box[0].screen_loc_centre_y = INFO_Y1 + 300;
		//	strcpy( tutor_box[1].text, "This is your Lair. Return your troop to it." );
			tutor_box[1].base_object_recno_no = allow_detect_obj_recno;
		}
		else
		if (cur_tutor_id == 4)
		{	
		// allow clicking and doing anything
			// detect all for final goal
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 0;
			tutor.update_tutorial_finish_count();
			close_box = 1;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		else
		if (cur_tutor_id == 5)
		{	
			// not allow clicking and doing anything
			// detect continue button
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 6;
			continue_box = 1;
			
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}

			load_in_game_box(cur_tutor_id, 6);
		//	strcpy( tutor_box[0].text, "This shows the goods that your Market is selling and how much money you are making." );
			tutor_box[0].screen_loc_centre_x = INFO_X1 + 100;
			tutor_box[0].screen_loc_centre_y = INFO_Y1 + 20;
			tutor_box[0].pointer_offset_x = 0;
			tutor_box[0].pointer_offset_y = -20;
		}
		else
		if (cur_tutor_id == 9)
		{	
		// allow clicking and doing anything
		// wait for the finish of Special Structure
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 5;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		else
		if (cur_tutor_id == 11)
		{	
		// allow clicking and doing anything
			// just click the continue box
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			// set this to enable the senario editor
		//	selectively_detect_flag = 0;
			selectively_detect_flag = 5;
			continue_box = 1;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		break;

	case 7:
		if (cur_tutor_id == 1)
		{	
			// allow clicking and doing anything
			// just click the continue box
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 5;
			continue_box = 1;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		else
		if (cur_tutor_id == 2)
		{	
			// not allow clicking and doing anything
			// detect seleting the first slave town
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 1;
			allow_detect_obj_recno = town_array[game.win_condition[1].para1]->base_obj_recno;
			allow_left_or_right_click = 1;
			allow_detect_button_rc = 1;

			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}

			load_in_game_box(cur_tutor_id, 7);
		//	strcpy( tutor_box[0].text, "This is your Slave Town. Click on it." );
			tutor_box[0].base_object_recno_no = allow_detect_obj_recno;
		}
		else
		if (cur_tutor_id == 5)
		{	
		// allow clicking and doing anything
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			// set this to enable the senario editor
		//	selectively_detect_flag = 0;
			selectively_detect_flag = 5;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		else
		if (cur_tutor_id == 9)
		{	
		// allow clicking and doing anything
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 5;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		else
		if (cur_tutor_id == 11)
		{	
			// allow clicking and doing anything
			// detect all for final battle
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 0;
			tutor.update_tutorial_finish_count();
			close_box = 1;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		break;
	
	case 8:
		if (cur_tutor_id == 1)
		{	
			// not allow clicking and doing anything
			// just click the continue box
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 5;
			continue_box = 1;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		else
		if (cur_tutor_id == 2)
		{	
			// not allow clicking and doing anything
			// detect seleting the lair
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 1;
			allow_detect_obj_recno = firm_array[game.win_condition[5].para1]->base_obj_recno;
			allow_left_or_right_click = 1;
			allow_detect_button_rc = 1;
			
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
			load_in_game_box(cur_tutor_id, 8);
		//	strcpy( tutor_box[0].text, "This shows your Life Points." );
			tutor_box[0].screen_loc_centre_x = REPU_BUTTON_X1+20;
			tutor_box[0].screen_loc_centre_y = REPU_BUTTON_Y1+20;
			if (current_display_mode.mode_id == MODE_ID_1024x768x16)
			{
				tutor_box[0].pointer_offset_x = -300;
				tutor_box[0].pointer_offset_y = 0;
			}
			else
			{
				tutor_box[0].pointer_offset_x = 0;
				tutor_box[0].pointer_offset_y = 100;
			}
			
		//	strcpy( tutor_box[1].text, "This shows your Money." );
			tutor_box[1].screen_loc_centre_x = CASH_X1+20;
			tutor_box[1].screen_loc_centre_y = CASH_Y1+10;
			if (current_display_mode.mode_id == MODE_ID_1024x768x16)
			{
				tutor_box[1].pointer_offset_x = -300;
				tutor_box[1].pointer_offset_y = 100;
			}
			else
			{
				tutor_box[1].pointer_offset_x = -100;
				tutor_box[1].pointer_offset_y = 100;
			}
			
		//	strcpy( tutor_box[2].text, "This shows the amount of tribute that you receive from your Slave Town." );
			tutor_box[2].screen_loc_centre_x = INFO_X1 + 150;
			tutor_box[2].screen_loc_centre_y = INFO_Y1 + 250;
		}
		else
		if (cur_tutor_id == 5)
		{	
			// allow clicking and doing anything
			// just click the continue box
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			// set this to enable the senario editor
		//	selectively_detect_flag = 0;
			selectively_detect_flag = 6;
			continue_box = 1;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
			load_in_game_box(cur_tutor_id, 8);
		//	strcpy( tutor_box[0].text, "This is the Tax Icon." );
			tutor_box[0].screen_loc_centre_x = INFO_X1 + 40;
			tutor_box[0].screen_loc_centre_y = INFO_Y1 + 250;
		//	strcpy( tutor_box[1].text, "This is your Town's Loyalty level." );
			tutor_box[1].screen_loc_centre_x = INFO_X1 + 145;
			tutor_box[1].screen_loc_centre_y = INFO_Y1 + 180;
		}
		else
		if (cur_tutor_id == 9)
		{	
			// not allow clicking and doing anything
			// detect clicking on button S_TRAIN
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 3;
			allow_left_or_right_click = 1;
			allow_detect_button_rc = 1;
			strcpy(allow_detect_button_name, "S_TRAIN");

			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		
			load_in_game_box(cur_tutor_id, 8);
		//	strcpy( tutor_box[0].text, "This is the Train Special Units Icon." );
			tutor_box[0].screen_loc_centre_x = INFO_X1 + 40;
			tutor_box[0].screen_loc_centre_y = INFO_Y1 + 300;
		}
		break;

	case 9:
		if (cur_tutor_id == 1)
		{	
			// allow clicking and doing anything
			// just click the continue box
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 5;
			continue_box = 1;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		else
		if (cur_tutor_id == 2)
		{	
			// not allow clicking and doing anything
			// and allow free access to lower left first 2 INFO buttons.
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 3;
			allow_left_or_right_click = 1;
			allow_detect_button_rc = 1;

			//	int x1 = INFO_X1 +13;
			//	int y1 = INFO_Y1 +235;
			//	int x2 = INFO_X1 +13;
			//	int y2 = INFO_Y1 +281;
			allow_detect_button_x1 = INFO_X1 +12;
			allow_detect_button_y1 = INFO_Y1 +280;
			allow_detect_button_x2 = INFO_X1 +15 + 2*BUTTON_DISTANCE;
			allow_detect_button_y2 = INFO_Y2;

			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}

			load_in_game_box(cur_tutor_id, 9);
		//	strcpy( tutor_box[0].text, "This is your next target." );
			tutor_box[0].base_object_recno_no = town_array[game.win_condition[2].para1]->base_obj_recno;
			
		//	strcpy( tutor_box[1].text, "This is the Breed Icon." );
			tutor_box[1].screen_loc_centre_x = INFO_X1 + 40;
			tutor_box[1].screen_loc_centre_y = INFO_Y1 + 300;
		}
		else
		if (cur_tutor_id == 5)
		{	
			// allow clicking and doing anything
			// just click the continue box
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			// set this to enable the senario editor
		//	selectively_detect_flag = 0;
			selectively_detect_flag = 5;
			continue_box = 1;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		else
		if (cur_tutor_id == 9)
		{
			// allow clicking and doing anything
			// just click the continue box
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 5;
			continue_box = 1;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		break;

	case 10:
		if (cur_tutor_id == 1)
		{	
			// not allow clicking and doing anything
			// detect clicking on button RETCAMP
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 3;
			allow_left_or_right_click = 1;
			allow_detect_button_rc = 1;
			strcpy(allow_detect_button_name, "RETCAMP");

			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
			
			load_in_game_box(cur_tutor_id, 10);
		//	strcpy( tutor_box[0].text, "This is the Return Icon." );
			tutor_box[0].screen_loc_centre_x = INFO_X1 + 144;
			tutor_box[0].screen_loc_centre_y = INFO_Y1 + 300;
		}
		else
		if (cur_tutor_id == 2)
		{
			// not allow clicking and doing anything
			// detect clicking on right click (attack) on a town
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 1;
			allow_detect_obj_recno = town_array[game.win_condition[2].para1]->base_obj_recno;
			allow_left_or_right_click = 2;
			allow_detect_button_rc = 2;

			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}

			load_in_game_box(cur_tutor_id, 10);
		//	strcpy( tutor_box[0].text, "This is an Independent Human Town. Right click to attack it." );
			tutor_box[0].base_object_recno_no = allow_detect_obj_recno;
		}
		else
		if (cur_tutor_id == 5)
		{	
			// allow clicking and doing anything
			// just click the continue box
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			// set this to enable the senario editor
		//	selectively_detect_flag = 0;
			selectively_detect_flag = 5;
			continue_box = 1;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		else
		if (cur_tutor_id == 9)
		{
			// allow clicking and doing anything
			// just click the continue box
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 5;
			continue_box = 1;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		break;
	
	case 11:
		if (cur_tutor_id == 1)
		{	
			// allow clicking and doing anything
			// just click the continue box
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 5;
			continue_box = 1;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		else
		if (cur_tutor_id == 2)
		{	
			// not allow clicking and doing anything
			// waiting for condition one - enslave an town - is true
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 6;
			
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
			load_in_game_box(cur_tutor_id, 11);
		//	strcpy( tutor_box[0].text, "Your troop is now enslaving this Town." );
			tutor_box[0].base_object_recno_no = town_array[game.win_condition[2].para1]->base_obj_recno;
		}
		else
		if (cur_tutor_id == 5)
		{	
		// allow clicking and doing anything
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			// set this to enable the senario editor
		//	selectively_detect_flag = 0;
			selectively_detect_flag = 5;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		else
		if (cur_tutor_id == 9)
		{	
		// allow clicking and doing anything
		// wait for the finish of War Factory
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 5;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		break;

	case 12:
		if (cur_tutor_id == 1)
		{	
			// not allow clicking and doing anything
			// find an independent town
			// centre the town
			// detect seleting the town
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 1;
			allow_detect_obj_recno = town_array[custom_search_independent_town(0, config.race_id, 101, 0, 101, 0)]->base_obj_recno;
			allow_left_or_right_click = 1;
			allow_detect_button_rc = 1;
			world.go_loc( town_array[custom_search_independent_town(0, config.race_id, 101, 0, 101, 0)]->center_x,
								town_array[custom_search_independent_town(0, config.race_id, 101, 0, 101, 0)]->center_y );
			
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}

			load_in_game_box(cur_tutor_id, 12);
		//	strcpy( tutor_box[0].text, "This is an Independent Town of same nationality as your kingdom" );
			tutor_box[0].base_object_recno_no = allow_detect_obj_recno;			
		}
		else
		if (cur_tutor_id == 2)
		{	
			// not allow clicking and doing anything
			// detect clicking on button F_RETCMP
			// detect right click on a firm
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 4;
			allow_detect_obj_recno = firm_array[game.win_condition[5].para1]->base_obj_recno;
			strcpy(allow_detect_button_name, "F_RETCMP");
			allow_left_or_right_click = 2;
			allow_detect_button_rc = 1;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}

			load_in_game_box(cur_tutor_id, 12);
		//	strcpy( tutor_box[0].text, "This is the Return Icon." );
			tutor_box[0].screen_loc_centre_x = INFO_X1 + 150;
			tutor_box[0].screen_loc_centre_y = INFO_Y1 + 300;
		//	strcpy( tutor_box[1].text, "This is your Lair. Return your troop to it." );
			tutor_box[1].base_object_recno_no = allow_detect_obj_recno;
		}
		else
		if (cur_tutor_id == 5)
		{	
			// not allow clicking and doing anything
			// just click the continue box
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 6;
			continue_box = 1;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
			load_in_game_box(cur_tutor_id, 12);
		//	strcpy( tutor_box[0].text, "This is the Grant Icon." );
			tutor_box[0].screen_loc_centre_x = INFO_X1 + 100;
			tutor_box[0].screen_loc_centre_y = INFO_Y1 + 250;
		}
		else
		if (cur_tutor_id == 9)
		{	
		// allow clicking and doing anything
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 5;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		break;

	case 13:
		if (cur_tutor_id == 1)
		{	
			// not allow clicking and doing anything
			// detect continue button
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 6;
			continue_box = 1;
			
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}

			load_in_game_box(cur_tutor_id, 13);
		//	strcpy( tutor_box[0].text, "This is the Independent Town's Resistance Level." );
			tutor_box[0].screen_loc_centre_x = INFO_X1 + 145;
			tutor_box[0].screen_loc_centre_y = INFO_Y1 + 180;
		}
		else
		if (cur_tutor_id == 2)
		{	
			// not allow clicking and doing anything
			// detect seleting the lair
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 1;
			allow_detect_obj_recno = firm_array[game.win_condition[5].para1]->base_obj_recno;
			allow_left_or_right_click = 1;
			allow_detect_button_rc = 1;
			
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
			load_in_game_box(cur_tutor_id, 13);
		//	strcpy( tutor_box[0].text, "Click on your Lair when your troop have all returned." );
			tutor_box[0].base_object_recno_no = allow_detect_obj_recno;
		}
		else
		if (cur_tutor_id == 5)
		{	
			// allow clicking and doing anything
			// just click the continue box
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			// set this to enable the senario editor
		//	selectively_detect_flag = 0;
			selectively_detect_flag = 5;
			continue_box = 1;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		else
		if (cur_tutor_id == 9)
		{	
			// not allow clicking and doing anything
			// detect clicking on button MAKEWEAP
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 3;
			allow_left_or_right_click = 1;
			allow_detect_button_rc = 1;
			strcpy(allow_detect_button_name, "MAKEWEAP");

			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		
			load_in_game_box(cur_tutor_id, 13);
		//	strcpy( tutor_box[0].text, "This is the Build Weapons Icon." );
			tutor_box[0].screen_loc_centre_x = INFO_X1 + 40;
			tutor_box[0].screen_loc_centre_y = INFO_Y1 + 300;
		}
		break;

	case 14:
		if (cur_tutor_id == 1)
		{	
			// not allow clicking and doing anything
			// detect seleting a thing in ZOOM view
			// no detect is allow in button
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 5;
				
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		else
		if (cur_tutor_id == 2)
		{	
			// not allow clicking and doing anything
			// detect any click inside the INFO area without INFO buttons
			// detect a right click on a unit icon
			// but not allow clicking the general position 
			// and not allow to click unit icon with lightness != 0
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 2;
			allow_detect_button_x1 = 0;
			allow_detect_button_y1 = 0;
			allow_detect_button_x2 = 0;
			allow_detect_button_y2 = 0;
			allow_unit_info_detect_rc = 0;
			allow_unit_info_detect_lightness = 0;
			not_allow_unit_info_detect_x2 = INFO_X1+20;
			not_allow_unit_info_detect_y2 = INFO_Y1+60;
										
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
			load_in_game_box(cur_tutor_id, 14);
		//	strcpy( tutor_box[0].text, "Click here to view the Leadership Level of all those in this Lair." );
			tutor_box[0].screen_loc_centre_x = INFO_X1 + 150;
			tutor_box[0].screen_loc_centre_y = INFO_Y1 + 210;
			tutor_box[0].pointer_offset_y = -200;
		}
		else
		if (cur_tutor_id == 5)
		{	
			// allow clicking and doing anything
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 0;
												
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
			load_in_game_box(cur_tutor_id, 14);
		//	strcpy( tutor_box[0].text, "This is the Kingdoms Button. You may Click here or press F1 on your keyboard to bring up the Diplomacy Menu. Press Esc to exit the Kingdoms menu." );
			if (current_display_mode.mode_id == MODE_ID_1024x768x16)
			{
				tutor_box[0].screen_loc_centre_x = 814;
				tutor_box[0].screen_loc_centre_y = 85;
				tutor_box[0].pointer_offset_x = -150;
			}
			else
			{
				tutor_box[0].screen_loc_centre_x = 40;
				tutor_box[0].screen_loc_centre_y = 30;
				tutor_box[0].pointer_offset_y = 300;
				tutor_box[0].pointer_offset_x = 200;
			}			
		}
		else
		if (cur_tutor_id == 9)
		{
			// allow clicking and doing anything
			// just click the continue box
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 5;
			continue_box = 1;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		break;
		
	case 15:
		if (cur_tutor_id == 1)
		{	
			// not allow clicking and doing anything
			// detect clicking on button F_PROMOT
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 3;
			allow_left_or_right_click = 1;
			allow_detect_button_rc = 1;
			strcpy(allow_detect_button_name, "BUILD");

			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		
			load_in_game_box(cur_tutor_id, 15);
		//	strcpy( tutor_box[0].text, "This is the Build Icon." );
			tutor_box[0].screen_loc_centre_x = INFO_X1 + 40;
			tutor_box[0].screen_loc_centre_y = INFO_Y1 + 250;
		}
		else
		if (cur_tutor_id == 2)
		{			
			// not allow clicking and doing anything
			// detect seleting a thing in ZOOM view
			// no detect is allow in button
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 1;
			allow_detect_obj_recno = -1; //allow select anything in the zoom area
			allow_left_or_right_click = 1; //but only left click
			allow_detect_button_rc = 1; //not really necessary as selectively_detect_flag will not allow clicking button
			
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		else
		if (cur_tutor_id == 5)
		{	
			// detect kingdom report
			// allow click on close box
			// display hint in front of the kingdom report
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 0;
			display_when_view_mode_actived = 1;
			enable_message();

			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		else
		if (cur_tutor_id == 9)
		{	
		// allow clicking and doing anything
		// wait for the finish of Seat of Power
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 5;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		break;
		
	case 16:
		if (cur_tutor_id == 1)
		{	
			// allow clicking and doing anything
			// just click the continue box
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 5;
			continue_box = 1;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		else
		if (cur_tutor_id == 2)
		{	
			// not allow clicking and doing anything
			// detect clicking on button F_PROMOT
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 3;
			allow_left_or_right_click = 1;
			allow_detect_button_rc = 1;
			strcpy(allow_detect_button_name, "F_PROMOT");

			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
			
			load_in_game_box(cur_tutor_id, 16);
		//	strcpy( tutor_box[0].text, "This is the Promote to Ordo Icon." );
			tutor_box[0].screen_loc_centre_x = INFO_X1 + 90;
			tutor_box[0].screen_loc_centre_y = INFO_Y1 + 250;
		}
		else
		if (cur_tutor_id == 5)
		{	
		// allow clicking and doing anything
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 5;
			set_view_mode(MODE_NORMAL);
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
			load_in_game_box(cur_tutor_id, 16);
		//	strcpy( tutor_box[0].text, "This shows the replay to your Trade proposal." );
			tutor_box[0].screen_loc_centre_x = 90;
			tutor_box[0].screen_loc_centre_y = VGA_HEIGHT - 30;
		}
		else
		if (cur_tutor_id == 9)
		{	
		// allow clicking and doing anything
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 5;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		break;

	case 17:
		if (cur_tutor_id == 1)
		{	
			// allow clicking and doing anything
			// detect on selecting any soldiers
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 5;	
			power.reset_selection();

			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		else
		if (cur_tutor_id == 2)
		{	
			// not allow clicking and doing anything
			// detect seleting the independent lair
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 1;
			allow_detect_obj_recno = firm_array[game.win_condition[3].para1]->base_obj_recno;
			allow_left_or_right_click = 1;
			allow_detect_button_rc = 1;
			
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
			load_in_game_box(cur_tutor_id, 17);
		//	strcpy( tutor_box[0].text, "Click on this Independent Lair." );
			tutor_box[0].base_object_recno_no = allow_detect_obj_recno;
		}
		else
		if (cur_tutor_id == 5)
		{	
			// not allow clicking and doing anything
			// detect clicking on button HIRECARA
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 3;
			allow_left_or_right_click = 1;
			allow_detect_button_rc = 1;
			strcpy(allow_detect_button_name, "HIRECARA");
			disable_message();

			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
			load_in_game_box(cur_tutor_id, 17);
		//	strcpy( tutor_box[0].text, "This is a Hire Caravan Icon." );
			tutor_box[0].screen_loc_centre_x = INFO_X1 + 40;
			tutor_box[0].screen_loc_centre_y = INFO_Y1 + 300;
		}
		else
		if (cur_tutor_id == 9)
		{	
			// not allow clicking and doing anything
			// and allow free access to lower left second INFO buttons.(ie. invoke god icon)
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 3;
			allow_left_or_right_click = 1;
			allow_detect_button_rc = 1;

		//	allow_detect_button_x1 = INFO_X1 +12 + BUTTON_DISTANCE; 
		//	allow_detect_button_y1 = INFO_Y1 +280;
		//	allow_detect_button_x2 = INFO_X1 +15 + 2*BUTTON_DISTANCE;
		//	allow_detect_button_y2 = INFO_Y2;

			// not allow clicking and doing anything
			// detect clicking on button HIRECARA
			strcpy(allow_detect_button_name, "GDINVOKE");
		
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}

			load_in_game_box(cur_tutor_id, 17);
		//	strcpy( tutor_box[0].text, "This is the Invoke Greater Being Icon." );
			tutor_box[0].screen_loc_centre_x = INFO_X1 + 90;
			tutor_box[0].screen_loc_centre_y = INFO_Y1 + 300;
		}
		break;

	case 18:
		if (cur_tutor_id == 1)
		{
			// not allow clicking and doing anything
			// detect clicking on button PROMOTE
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 3;
			allow_left_or_right_click = 1;
			allow_detect_button_rc = 1;
			strcpy(allow_detect_button_name, "PROMOTE");
			
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		
			load_in_game_box(cur_tutor_id, 18);
		//	strcpy( tutor_box[0].text, "This is the Promote to Ordo Icon." );
			tutor_box[0].screen_loc_centre_x = INFO_X1 + 90;
			tutor_box[0].screen_loc_centre_y = INFO_Y1 + 250;
		}
		else
		if (cur_tutor_id == 2)
		{			
			// not allow clicking and doing anything
			// detect seleting a thing in ZOOM view
			// no detect is allow in button
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 1;
			allow_detect_obj_recno = -1; //allow select anything in the zoom area
			allow_left_or_right_click = 1; //but only left click
			allow_detect_button_rc = 1; //not really necessary as selectively_detect_flag will not allow clicking button
			
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		else
		if (cur_tutor_id == 5)
		{	
		// allow clicking and doing anything
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 5;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		else
		if (cur_tutor_id == 9)
		{	
		// allow clicking and doing anything
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
		//	disable_fast_build();
			selectively_detect_flag = 5;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		break;
		
	case 19:
		if (cur_tutor_id == 1)
		{	
			// allow clicking and doing anything
			// just click the continue box
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 5;
			continue_box = 1;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		else
		if (cur_tutor_id == 2)
		{	
			// not allow clicking and doing anything
			// detect clicking on button BUILD
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 3;
			allow_left_or_right_click = 1;
			allow_detect_button_rc = 1;
			strcpy(allow_detect_button_name, "BUILD");

			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
			
			load_in_game_box(cur_tutor_id, 19);
		//	strcpy( tutor_box[0].text, "This is the Build Icon." );
			tutor_box[0].screen_loc_centre_x = INFO_X1 + 40;
			tutor_box[0].screen_loc_centre_y = INFO_Y1 + 250;
		}
		else
		if (cur_tutor_id == 5)
		{	
			// not allow clicking and doing anything
			// just click the continue box
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 6;
			continue_box = 1;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
			load_in_game_box(cur_tutor_id, 19);
		//	strcpy( tutor_box[0].text, "This is the New Icon." );
			tutor_box[0].screen_loc_centre_x = INFO_X1 + 200;
			tutor_box[0].screen_loc_centre_y = INFO_Y1 + 180;
		}
		else
		if (cur_tutor_id == 9)
		{	
			// not allow clicking and doing anything
			// but allow right clicking anything in ZOOM VIEW
			// just click the continue box
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
		//	selectively_detect_flag = 6;
			
			selectively_detect_flag = 1;
			allow_detect_obj_recno = -1; //allow select anything in the zoom area
			allow_left_or_right_click = 2; //but only right click
			allow_detect_button_rc = 1; //not really necessary as selectively_detect_flag will not allow clicking button
			
			continue_box = 1;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
			load_in_game_box(cur_tutor_id, 19);
		//	strcpy( tutor_box[0].text, "This text describes how to use your Greater Being." );
			tutor_box[0].screen_loc_centre_x = INFO_X1 + 80;
			tutor_box[0].screen_loc_centre_y = INFO_Y1 + 60;
		//	strcpy( tutor_box[1].text, "These are the Prayer Points that  sustain your Greater Being." );
			tutor_box[1].screen_loc_centre_x = INFO_X1 + 150;
			tutor_box[1].screen_loc_centre_y = INFO_Y1 + 203;
			tutor_box[1].pointer_offset_y = -30;
			tutor_box[1].pointer_offset_x = 0;
		}
		break;
	
	case 20:
		if (cur_tutor_id == 1)
		{	
			// allow clicking and doing anything
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 5;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		else
		if (cur_tutor_id == 2)
		{	
			// allow clicking and doing anything
			// wait until a lair linked to the independent lair is built
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 5;
						
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		else
		if (cur_tutor_id == 5)
		{	
		// allow clicking and doing anything
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 5;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		else
		if (cur_tutor_id == 9)
		{	
		// allow clicking and doing anything
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 5;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		break;
		
	case 21:
		if (cur_tutor_id == 1)
		{	
			// allow clicking and doing anything
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 5;
			
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
				if (textBlock->objective_no != current_objective)
					continue;
				cur_text_block_id = i+1;
				break;
			}
		}
		else
		if (cur_tutor_id == 2)
		{	
			// not allow clicking and doing anything
			// detect seleting the independent lair
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 1;
			allow_detect_obj_recno = firm_array[game.win_condition[3].para1]->base_obj_recno;
			allow_left_or_right_click = 1;
			allow_detect_button_rc = 1;
			
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
			load_in_game_box(cur_tutor_id, 21);
		//	strcpy( tutor_box[0].text, "Click on this Independent Lair." );
			tutor_box[0].base_object_recno_no = allow_detect_obj_recno;
		}
		else
		if (cur_tutor_id == 5)
		{	
		// allow clicking and doing anything
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 5;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		else
		if (cur_tutor_id == 9)
		{	
			// not allow clicking and doing anything
			// detect clicking on button BLESS
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
		//	set_fast_build();
			selectively_detect_flag = 3;
			allow_left_or_right_click = 1;
			allow_detect_button_rc = 1;
			for( int i=1 ; i<=unit_array.size() ; i++ )
			{
				if( unit_array.is_deleted(i) )
					continue;

				Unit* unitPtr = unit_array[i];
				if( unitPtr->nation_recno != (~nation_array)->nation_recno	)
					continue;

				if( unit_res[unitPtr->unit_id]->unit_class != UNIT_CLASS_GOD )
					continue;
				
				unitPtr->resign(COMMAND_PLAYER);		
			}

//			allow_detect_button_x1 = INFO_X1 +12 + 2*BUTTON_DISTANCE; 
//			allow_detect_button_y1 = INFO_Y1 +280;
//			allow_detect_button_x2 = INFO_X1 +15 + 3*BUTTON_DISTANCE;
//			allow_detect_button_y2 = INFO_Y2;
			strcpy(allow_detect_button_name, "BLESS");

			// reset cur_text_block_id for the new objective
			for (i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}

			load_in_game_box(cur_tutor_id, 21);
		//	strcpy( tutor_box[1].text, "This is the Special Blessing Icon. It is only visible when your Prayer Points are full." );
			tutor_box[0].screen_loc_centre_x = INFO_X1 + 150;
			tutor_box[0].screen_loc_centre_y = INFO_Y1 + 300;
		}
		break;

	case 22:
		if (cur_tutor_id == 1)
		{	
			// not allow clicking and doing anything
			// detect continue button
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 6;
			continue_box = 1;
			
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
				if (textBlock->objective_no != current_objective)
					continue;
				cur_text_block_id = i+1;
				break;
			}
			load_in_game_box(cur_tutor_id, 22);
		//	strcpy( tutor_box[0].text, "This is the town that you want to win." );
			if (!town_array.is_deleted(par1))
				tutor_box[0].base_object_recno_no = town_array[par1]->base_obj_recno;
		//	strcpy( tutor_box[1].text, "This shows the falling level of Resistance in this Independent Town." );
			tutor_box[1].screen_loc_centre_x = INFO_X1 + 145;
			tutor_box[1].screen_loc_centre_y = INFO_Y1 + 180;
		}
		else
		if (cur_tutor_id == 2)
		{	
			// not allow clicking and doing anything
			// detect clicking on button F_GRANT
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 3;
			allow_left_or_right_click = 1;
			allow_detect_button_rc = 1;
			strcpy(allow_detect_button_name, "F_GRANT");

			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		
			load_in_game_box(cur_tutor_id, 22);
		//	strcpy( tutor_box[0].text, "This is the Payoff Icon." );
			tutor_box[0].screen_loc_centre_x = INFO_X1 + 40;
			tutor_box[0].screen_loc_centre_y = INFO_Y1 + 250;
		}
		else
		if (cur_tutor_id == 5)
		{	
			// not allow clicking and doing anything
			// just click the continue box
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 6;
			continue_box = 1;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
			load_in_game_box(cur_tutor_id, 22);
		//	strcpy( tutor_box[0].text, "This shows the selection of Goods for your Caravan's load." );
			tutor_box[0].screen_loc_centre_x = INFO_X1 + 100;
			tutor_box[0].screen_loc_centre_y = INFO_Y1 + 140;
		}
		else
		if (cur_tutor_id == 9)
		{	
			// allow clicking and doing anything
			// just click the continue box
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 5;
			continue_box = 1;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		break;
	
	case 23:
		if (cur_tutor_id == 1)
		{	
			// allow clicking and doing anything
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 5;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
			load_in_game_box(cur_tutor_id, 23);
		//	strcpy( tutor_box[0].text, "This is the town that you want to win." );
			if (!town_array.is_deleted(par1))
				tutor_box[0].base_object_recno_no = town_array[par1]->base_obj_recno;
		}
		else
		if (cur_tutor_id == 2)
		{
			// not allow clicking and doing anything
			// just allow clicking the "YES" button inside the INFO view area
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 2;
			not_allow_info_detect_x1 = (INFO_X2-((INFO_X2-INFO_X1)>>1));
			not_allow_info_detect_x2 = INFO_X2;
			not_allow_info_detect_y1 = INFO_Y1;
			not_allow_info_detect_y2 = INFO_Y2;

			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		else
		if (cur_tutor_id == 5)
		{	
		// allow clicking and doing anything
			// just click the continue box
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			// set this to enable the senario editor
		//	selectively_detect_flag = 0;
			selectively_detect_flag = 5;
			continue_box = 1;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		else
		if (cur_tutor_id == 9)
		{	
		// allow clicking and doing anything
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 5;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		break;

	case 24:
		if (cur_tutor_id == 1)
		{	
			// allow clicking and doing anything
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 5;
			if (!firm_array.is_deleted(game.win_condition[3].para1))
				world.go_loc( firm_array[game.win_condition[3].para1]->center_x,
								firm_array[game.win_condition[3].para1]->center_y );
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
			load_in_game_box(cur_tutor_id, 24);
		//	strcpy( tutor_box[0].text, "This is a Fryhtan Lair linked to a still Independent Town." );
			tutor_box[0].base_object_recno_no = firm_array[game.win_condition[3].para1]->base_obj_recno;
		}
		else
		if (cur_tutor_id == 2)
		{	
			// allow clicking and doing anything
			// detect all for final battle
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 5;
			tutor.update_tutorial_finish_count();
			close_box = 1;
									
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}			
		}
		else
		if (cur_tutor_id == 5)
		{	
		// allow clicking and doing anything
			// just click the continue box
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			// set this to enable the senario editor
		//	selectively_detect_flag = 0;
			selectively_detect_flag = 5;
			continue_box = 1;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		else
		if (cur_tutor_id == 9)
		{	
			// not allow clicking and doing anything
			// just click the continue box
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 6;
			continue_box = 1;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
			load_in_game_box(cur_tutor_id, 24);
		//	strcpy( tutor_box[0].text, "This is where Mercenary Soldiers gather in the Inn." );
			tutor_box[0].screen_loc_centre_x = INFO_X1 + 100;
			tutor_box[0].screen_loc_centre_y = INFO_Y1 + 40;

		//	strcpy( tutor_box[1].text, "This is the Hire Mercenary Icon." );
			tutor_box[1].screen_loc_centre_x = INFO_X1 + 40;
			tutor_box[1].screen_loc_centre_y = INFO_Y1 + 250;		
		}
		break;
		
	case 25:
		if (cur_tutor_id == 1)
		{	
			// allow clicking and doing anything
			// detect all for final battle
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 5;
			tutor.update_tutorial_finish_count();
			close_box = 1;

			if (!firm_array.is_deleted(game.win_condition[4].para1))
				world.go_loc( firm_array[game.win_condition[4].para1]->center_x,
								firm_array[game.win_condition[4].para1]->center_y );
			
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
			load_in_game_box(cur_tutor_id, 25);
		//	strcpy( tutor_box[0].text, "This is a Fryhtan Lair linked to an Independent Town." );
			tutor_box[0].base_object_recno_no = firm_array[game.win_condition[4].para1]->base_obj_recno;
		}
		else
		if (cur_tutor_id == 5)
		{	
		// allow clicking and doing anything
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 5;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		} 
		else
		if (cur_tutor_id == 9)
		{	
		// allow clicking and doing anything
			// just click the continue box
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			// set this to enable the senario editor
		//	selectively_detect_flag = 0;
			selectively_detect_flag = 5;
			continue_box = 1;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		break;

	case 26:
		if (cur_tutor_id == 5)
		{	
			// not allow clicking and doing anything
			// detect clicking on button CARACOPY
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 3;
			allow_left_or_right_click = 1;
			allow_detect_button_rc = 1;
			strcpy(allow_detect_button_name, "CARACOPY");
			
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
			load_in_game_box(cur_tutor_id, 26);
		//	strcpy( tutor_box[0].text, "This is the Clone Caravan Icon." );
			tutor_box[0].screen_loc_centre_x = INFO_X1 + 40;
			tutor_box[0].screen_loc_centre_y = INFO_Y1 + 300;
		}
		else
		if (cur_tutor_id == 9)
		{	
		// allow clicking and doing anything
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 5;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		break;

	case 27:
		if (cur_tutor_id == 5)
		{	
		// allow clicking and doing anything
			// just click the continue box
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			// set this to enable the senario editor
		//	selectively_detect_flag = 0;
			selectively_detect_flag = 5;
			continue_box = 1;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		else
		if (cur_tutor_id == 9)
		{	
			// not allow clicking and doing anything
			// just click the continue box
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 6;
			continue_box = 1;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
			load_in_game_box(cur_tutor_id, 27);
		//	strcpy( tutor_box[0].text, "This is where Mercenary Spies gather in the Inn." );
			tutor_box[0].screen_loc_centre_x = INFO_X1 + 150;
			tutor_box[0].screen_loc_centre_y = INFO_Y1 + 100;
		}
		break;
		
	case 28:
		if (cur_tutor_id == 5)
		{	
			// detect all for final goal
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 0;
			tutor.update_tutorial_finish_count();
			close_box = 1;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		else
		if (cur_tutor_id == 9)
		{	
		// allow clicking and doing anything
			// just click the continue box
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			// set this to enable the senario editor
		//	selectively_detect_flag = 0;
			selectively_detect_flag = 5;
			continue_box = 1;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		break;

	case 29:
		if (cur_tutor_id == 9)
		{	
		// allow clicking and doing anything
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 5;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		break;

	case 30:
		if (cur_tutor_id == 9)
		{	
			// not allow clicking and doing anything
			// and allow free access to lower left first three INFO buttons.(ie. all train button in spy college)
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 3;
			allow_left_or_right_click = 1;
			allow_detect_button_rc = 1;

			allow_detect_button_x1 = INFO_X1 +12; 
			allow_detect_button_y1 = INFO_Y1 +280;
			allow_detect_button_x2 = INFO_X1 +15 + 3*BUTTON_DISTANCE;
			allow_detect_button_y2 = INFO_Y2;
			
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}

			load_in_game_box(cur_tutor_id, 30);
		//	strcpy( tutor_box[1].text, "Click on the following 3 Spy Training Icon to train different kinds of Spies." );
			tutor_box[0].screen_loc_centre_x = INFO_X1 + 90;
			tutor_box[0].screen_loc_centre_y = INFO_Y1 + 300;
		}
		break;

	case 31:
		if (cur_tutor_id == 9)
		{	
		// allow clicking and doing anything
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 5;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		break;

	case 32:
		if (cur_tutor_id == 9)
		{	
			// not allow clicking and doing anything
			// just click the continue box
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 6;
			continue_box = 1;

			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		
			load_in_game_box(cur_tutor_id, 32);
		//	strcpy( tutor_box[0].text, "Click on this to see more Spying options." );
			tutor_box[0].screen_loc_centre_x = INFO_X1 + 150;
			tutor_box[0].screen_loc_centre_y = INFO_Y1 + 250;
		}
		break;

	case 33:
		if (cur_tutor_id == 9)
		{	
		// allow clicking and doing anything
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 5;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		break;

	case 34:
		if (cur_tutor_id == 9)
		{	
			// not allow clicking and doing anything
			// allow any click inside the INFO area without INFO buttons
			// just click the continue box
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 2;
			allow_detect_button_x1 = 0;
			allow_detect_button_y1 = 0;
			allow_detect_button_x2 = 0;
			allow_detect_button_y2 = 0;
			continue_box = 1;
								
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
			load_in_game_box(cur_tutor_id, 34);
		//	strcpy( tutor_box[0].text, "Click on one of these to change your Spy's color." );
			tutor_box[0].screen_loc_centre_x = INFO_X1 + 160;
			tutor_box[0].screen_loc_centre_y = INFO_Y1 + 90;
		}
		break;

	case 35:
		if (cur_tutor_id == 9)
		{	
			// not allow clicking and doing anything
			// detect clicking on button SPYNOTI0
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 3;
			allow_left_or_right_click = 1;
			allow_detect_button_rc = 1;
			strcpy(allow_detect_button_name, "SPYNOTI0");

			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		
			load_in_game_box(cur_tutor_id, 35);
		//	strcpy( tutor_box[0].text, "This is the Sneak Icon." );
			tutor_box[0].screen_loc_centre_x = INFO_X1 + 200;
			tutor_box[0].screen_loc_centre_y = INFO_Y1 + 300;
		}
		break;

	case 36:
		if (cur_tutor_id == 9)
		{	
			// not allow clicking and doing anything
			// detect clicking on button SPYNOTI1
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 3;
			allow_left_or_right_click = 1;
			allow_detect_button_rc = 1;
			strcpy(allow_detect_button_name, "SPYNOTI1");

			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		
			load_in_game_box(cur_tutor_id, 36);
		//	strcpy( tutor_box[0].text, "This is now the Surrender Icon." );
			tutor_box[0].screen_loc_centre_x = INFO_X1 + 200;
			tutor_box[0].screen_loc_centre_y = INFO_Y1 + 300;
		}
		break;

	case 37:
		if (cur_tutor_id == 9)
		{	
			// not allow clicking and doing anything
			// just click the continue box
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 6;
			continue_box = 1;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
			load_in_game_box(cur_tutor_id, 37);
		//	strcpy( tutor_box[0].text, "This is the Spy Retire Icon." );
			tutor_box[0].screen_loc_centre_x = INFO_X1 + 200;
			tutor_box[0].screen_loc_centre_y = INFO_Y1 + 250;
		}
		break;

	case 38:
		if (cur_tutor_id == 9)
		{	
		// allow clicking and doing anything
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 5;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		break;

	case 39:
		if (cur_tutor_id == 9)
		{	
			// not allow clicking and doing anything
			// detect clicking on button SPYMENU
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 3;
			allow_left_or_right_click = 1;
			allow_detect_button_rc = 1;
			strcpy(allow_detect_button_name, "SPYMENU");

			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		
			load_in_game_box(cur_tutor_id, 39);
		//	strcpy( tutor_box[0].text, "Click on this to see more Spying options." );
			tutor_box[0].screen_loc_centre_x = INFO_X1 + 150;
			tutor_box[0].screen_loc_centre_y = INFO_Y1 + 250;
		}
		break;

	case 40:
		if (cur_tutor_id == 9)
		{	
			// not allow clicking and doing anything
			// detect clicking on button CHG-TASK
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 3;
			allow_left_or_right_click = 1;
			allow_detect_button_rc = 1;
			strcpy(allow_detect_button_name, "CHG-MISS");

			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		
			load_in_game_box(cur_tutor_id, 40);
		//	strcpy( tutor_box[0].text, "This is the Change Task Icon." );
			tutor_box[0].screen_loc_centre_x = INFO_X1 + 90;
			tutor_box[0].screen_loc_centre_y = INFO_Y1 + 250;
		}
		break;

	case 41:
		if (cur_tutor_id == 9)
		{	
		// allow clicking and doing anything
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 5;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		break;

	case 42:
		if (cur_tutor_id == 9)
		{	
			// not allow clicking and doing anything
			// just click the continue box
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 6;
			continue_box = 1;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
			load_in_game_box(cur_tutor_id, 42);
		//	strcpy( tutor_box[0].text, "This is the Bribe Icon." );
			tutor_box[0].screen_loc_centre_x = INFO_X1 + 150;
			tutor_box[0].screen_loc_centre_y = INFO_Y1 + 250;
		}
		break;

	case 43:
		if (cur_tutor_id == 9)
		{	
		// allow clicking and doing anything
			// click the continue box and select a firm with a spy inside
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			// set this to enable the senario editor
		//	selectively_detect_flag = 0;
			selectively_detect_flag = 5;
			continue_box = 1;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		break;

	case 44:
		if (cur_tutor_id == 9)
		{	
			// not allow clicking and doing anything
			// just click the continue box
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 6;
			continue_box = 1;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
			load_in_game_box(cur_tutor_id, 44);
		//	strcpy( tutor_box[0].text, "This is the Capture Icon." );
			tutor_box[0].screen_loc_centre_x = INFO_X1 + 40;
			tutor_box[0].screen_loc_centre_y = INFO_Y1 + 300;
		}
		break;

	case 45:
		if (cur_tutor_id == 9)
		{	
		// allow clicking and doing anything
			// just click the continue box
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			// set this to enable the senario editor
		//	selectively_detect_flag = 0;
			selectively_detect_flag = 5;
			continue_box = 1;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		break;
	
	case 46:
		if (cur_tutor_id == 9)
		{	
		// allow clicking and doing anything
			// just click the continue box
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			// set this to enable the senario editor
		//	selectively_detect_flag = 0;
			selectively_detect_flag = 5;
			continue_box = 1;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		break;

	case 47:
		if (cur_tutor_id == 9)
		{	
		// allow clicking and doing anything
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 5;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		break;

	case 48:
		if (cur_tutor_id == 9)
		{	
			// not allow clicking and doing anything
			// detect clicking on button SPYMENU
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 3;
			allow_left_or_right_click = 1;
			allow_detect_button_rc = 1;
			strcpy(allow_detect_button_name, "SPYMENU");

			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		
			load_in_game_box(cur_tutor_id, 48);
		//	strcpy( tutor_box[0].text, "Click on this to see more Spying options." );
			tutor_box[0].screen_loc_centre_x = INFO_X1 + 90;
			tutor_box[0].screen_loc_centre_y = INFO_Y1 + 250;
		}
		break;

	case 49:
		if (cur_tutor_id == 9)
		{	
			// not allow clicking and doing anything
			// just click the continue box
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 6;
			continue_box = 1;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
			load_in_game_box(cur_tutor_id, 49);
		//	strcpy( tutor_box[0].text, "This is the Assassinate Icon." );
			tutor_box[0].screen_loc_centre_x = INFO_X1 + 90;
			tutor_box[0].screen_loc_centre_y = INFO_Y1 + 300;
		}
		break;

	case 50:
		if (cur_tutor_id == 9)
		{	
			// not allow clicking and doing anything
			// just click the continue box
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 6;
			continue_box = 1;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
			load_in_game_box(cur_tutor_id, 50);
		//	strcpy( tutor_box[0].text, "This is the Steal Technology Icon." );
			tutor_box[0].screen_loc_centre_x = INFO_X1 + 150;
			tutor_box[0].screen_loc_centre_y = INFO_Y1 + 250;
		}
		break;

	case 51:
		if (cur_tutor_id == 9)
		{	
			// not allow clicking and doing anything
			// just click the continue box
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 6;
			continue_box = 1;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
			load_in_game_box(cur_tutor_id, 51);
		//	strcpy( tutor_box[0].text, "This is the View Information Icon." );
			tutor_box[0].screen_loc_centre_x = INFO_X1 + 150;
			tutor_box[0].screen_loc_centre_y = INFO_Y1 + 300;
		}
		break;

	case 52:
		if (cur_tutor_id == 9)
		{	
			// not allow clicking and doing anything
			// just click the continue box
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 6;
			continue_box = 1;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
			load_in_game_box(cur_tutor_id, 52);
		//	strcpy( tutor_box[0].text, "This is the Create Incident Icon." );
			tutor_box[0].screen_loc_centre_x = INFO_X1 + 200;
			tutor_box[0].screen_loc_centre_y = INFO_Y1 + 250;
		}
		break;

	case 53:
		if (cur_tutor_id == 9)
		{	
			// not allow clicking and doing anything
			// just click the continue box
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 6;
			continue_box = 1;
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
			load_in_game_box(cur_tutor_id, 53);
		//	strcpy( tutor_box[0].text, "This is the Mobilize Spy Icon." );
			tutor_box[0].screen_loc_centre_x = INFO_X1 + 40;
			tutor_box[0].screen_loc_centre_y = INFO_Y1 + 300;
		}
		break;
	
	case 54:
		if (cur_tutor_id == 9)
		{	
			// allow clicking and doing anything
			// detect all for final battle
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			selectively_detect_flag = 0;
			tutor.update_tutorial_finish_count();
			close_box = 1;

			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
		}
		break;
	
	default:
		if (objestiveRecNo <= intro_text_block_count)
		{
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+objestiveRecNo-1;
			current_objective = objestiveRecNo;
			reset_detect_variables();
			
			// reset cur_text_block_id for the new objective
			for (int i = 0; i <text_block_count; i++)
			{
				TutorTextBlock* textBlock = text_block_array+i;
			
				if (textBlock->objective_no != current_objective)
					continue;
				
				cur_text_block_id = i+1;
				break;
			}
			// if no hints for that objective is found,
			// objective will be display in the hint position
			if ( i < text_block_count && strcmp(tutorTextBlock->intro_ptr,"") != 0)
				disp_msg(0, 3, tutorTextBlock->intro_ptr);
		}
	}
}
// ------- End of function Tutor::disp_objective_text -----//


// ------- Begin of function Tutor::disp_result_text -----//
void Tutor::disp_result_text(int objestiveRecNo)
{
	switch (objestiveRecNo)
	{
	case 0:
		// called at the start of tutorial
		// should not display any text
		return;
	default:
		if (objestiveRecNo <= result_text_block_count)
		{
			TutorTextBlock* tutorTextBlock = text_block_array+text_block_count+
															intro_text_block_count+objestiveRecNo-1;
			if (strcmp(tutorTextBlock->result_ptr,"") != 0)
				disp_msg(0, 1, tutorTextBlock->result_ptr);
		}
	}
	// how to check
	// how to load text file
}
// ------- End of function Tutor::disp_result_text -----//

// ------- Begin of function Tutor::detect_button -----//
int Tutor::detect_button(short x1, short y1, short x2, short y2, char* buttonName, short buttonRc)
{
	if(
		((x1 <= detect_button_x1) &&
		 (y1 <= detect_button_y1) &&
		 (x2 >= detect_button_x2) &&
		 (y2 >= detect_button_y2))
				||
		(strcmp(detect_button_name, buttonName) == 0)
	  )
		if (detect_button_rc == buttonRc)
			return 1;
	
	strcpy( detect_button_name, "" );
	detect_button_x1 = 0;
	detect_button_y1 = 0;
	detect_button_x2 = 0;
	detect_button_y2 = 0;
	detect_button_rc = 0;
	return 0;
}
// ------- End of function Tutor::detect_button -----//

// ------- Begin of function Tutor::allow_mouse_click -----//
int Tutor::allow_mouse_click(short x, short y, short pressOrRelease, short rightOrLeft)
{
	// selectively_detect_flag has no effect in MENU and OPTION MENU
	// 0 = allow detect any
	// 1 = allow detect in ZOOM VIEW specified objects
	// 2 = allow detect in INFO VIEW 
	// 3 = allow detect button area only
	// 4 = allow detect in ZOOM VIEW specified objects only and button area only
	// 5 = do not allow any detect in screen
	
	detect_obj_recno = 0;
//	if (pressOrRelease >= 0)
		detect_press_or_release_flag = 0;
//	if (rightOrLeft >= 0)
		detect_left_or_right_flag = 0;

	if (bypass_allow_mouse_click == 1)
		return 1;
				
	if (!selectively_detect_flag || in_game_menu.is_active() || 
		option_menu.is_active() || power.win_opened || box.opened_flag)
		return 1;

// no need to place in here, the following checking will 
// make the return number to zero
//	if (selectively_detect_flag == 6)
//		return 0;
	
	if( x >= ZOOM_X1 && y >= ZOOM_Y1 && x <= ZOOM_X2 && y <= ZOOM_Y2 && selectively_detect_flag == 5 )
		return 1;
	
	int xLoc, yLoc;
	char mobileType;
	if( x >= ZOOM_X1 && y >= ZOOM_Y1 && x <= ZOOM_X2 && y <= ZOOM_Y2 && 
		(selectively_detect_flag == 1 || selectively_detect_flag == 4 || selectively_detect_flag == 5))
	{
		Location *locPtr = power.test_detect(x, y, &mobileType, &xLoc, &yLoc);
		if( locPtr && locPtr->base_obj_recno() )
		{
			if ((allow_detect_obj_recno == locPtr->base_obj_recno() || allow_detect_obj_recno < 0) &&
				(allow_left_or_right_click == rightOrLeft || !allow_left_or_right_click))
			{
				detect_obj_recno = locPtr->base_obj_recno();
			//	if (pressOrRelease >= 0)
					detect_press_or_release_flag = pressOrRelease;
			//	if (pressOrRelease >= 0)
					detect_left_or_right_flag = rightOrLeft;
				return 1;
			}
		}
	}
	else
	if( x >= INFO_X1 && y >= INFO_Y1 && x <= INFO_X2 && y <= INFO_Y2 && 
		(selectively_detect_flag == 2 || selectively_detect_flag == 5) )
		if ( x >= not_allow_info_detect_x1 && y >= not_allow_info_detect_y1 &&
				 x <= not_allow_info_detect_x2  && y <= not_allow_info_detect_y2)
			return 0;
		else
			return 1;
	else
	if( x >= INFO_X1 +13 && y >= INFO_Y1 +235 && x <= INFO_X2 && y <= INFO_Y2 && 
		(selectively_detect_flag == 3 || selectively_detect_flag == 4) )		
		return 1;
	else
	if( x >= MENU_BUTTON_X1 && y >= MENU_BUTTON_Y1 && x <= MENU_BUTTON_X1+23 && y <= MENU_BUTTON_Y1+76 )
		return 1;

	return 0;
}
// ------- End of function Tutor::allow_mouse_click -----//

// ------- Begin of function Tutor::allow_button_click -----//
int Tutor::allow_button_click(short x1, short y1, short x2, short y2, char* buttonName, short buttonRc)
{
	if (bypass_allow_button_click == 1)
		return 1;	

	if(( x1 >= MENU_BUTTON_X1 && y1 >= MENU_BUTTON_Y1 && x2 <= MENU_BUTTON_X1+27 && y2 <= MENU_BUTTON_Y1+80 )
		||
		(in_game_menu.is_active() || option_menu.is_active() || 
		power.win_opened || box.opened_flag))
	{	
		strcpy( detect_button_name, "" );
		detect_button_x1 = 0;
		detect_button_y1 = 0;
		detect_button_x2 = 0;
		detect_button_y2 = 0;
		detect_button_rc = 0;
		return 1;
	}

	if(
		((x1 >= allow_detect_button_x1) &&
		 (y1 >= allow_detect_button_y1) &&
		 (x2 <= allow_detect_button_x2) &&
		 (y2 <= allow_detect_button_y2) &&
		 (strcmp(allow_detect_button_name, "") == 0)
		)
				||
//		(strcmp(allow_detect_button_name, buttonName) == 0 &&
//			strcmp(buttonName, "") != 0)
		strcmp(allow_detect_button_name, buttonName) == 0
	  )
	  // allow detect for first click only
	  if((strcmp( detect_button_name, "" ) == 0 &&
			detect_button_x1 == 0 &&
			detect_button_y1 == 0 &&
			detect_button_x2 == 0 &&
			detect_button_y2 == 0 &&
			detect_button_rc == 0 &&
			(allow_detect_button_rc == buttonRc || !allow_detect_button_rc))
			||
			selectively_detect_flag == 0 ||
			selectively_detect_flag == 5)
	  {
			strcpy( detect_button_name, buttonName );
			detect_button_x1 = x1;
			detect_button_y1 = y1;
			detect_button_x2 = x2;
			detect_button_y2 = y2;
			detect_button_rc = buttonRc;	
			return 1;
	  }
	
	 return 0;
}
// ------- End of function Tutor::allow_button_click -----//	

// ------- Begin of function Tutor::allow_unit_icon_click -----//
int Tutor::allow_unit_icon_click(short x1, short y1, UnitInfo *unitInfo, int lightness, short buttonRc)
{
	if (not_allow_unit_info_detect_x2 > x1 &&	not_allow_unit_info_detect_y2 > y1)
		return 0;

	if (allow_unit_info_detect_rc > 0 && allow_unit_info_detect_rc != buttonRc)
		return 0;

	if (allow_unit_info_detect_lightness >= 0 && lightness != allow_unit_info_detect_lightness)
		return 0;

	detect_unit_info_rc = buttonRc;
	return 1;
}
// ------- End of function Tutor::allow_unit_icon_click -----//

// ------- Begin of function Tutor::reset_detect_variables -----//
void Tutor::reset_detect_variables()
{
	// allow all button click and click on all area
	strcpy( detect_button_name, "" );
	detect_button_x1 = 0;
	detect_button_y1 = 0;
	detect_button_x2 = 0;
	detect_button_y2 = 0;
	detect_button_rc = 0;
	
	strcpy( allow_detect_button_name, "" );
	allow_detect_button_x1 = 0;
	allow_detect_button_y1 = 0;
	allow_detect_button_x2 = VGA_WIDTH;
	allow_detect_button_y2 = VGA_HEIGHT;
	allow_detect_button_rc = 0;
	
	not_allow_info_detect_x1 = 0;
	not_allow_info_detect_y1 = 0;
	not_allow_info_detect_x2 = 0;
	not_allow_info_detect_y2 = 0;

	allow_unit_info_detect_rc = 0;
	allow_unit_info_detect_lightness = -1;
	not_allow_unit_info_detect_x2 = 0;
	not_allow_unit_info_detect_y2 = 0;
	detect_unit_info_rc = 0;
	
	detect_obj_recno = 0;
	detect_press_or_release_flag = 0; // 1 = press, 2 = release
	detect_left_or_right_flag = 0;	// 1 = left, 2 = right
	allow_detect_obj_recno = 0;
	allow_left_or_right_click = 0;

	for (int i = 0; i < MAX_IN_GAME_BOX_PER_OBJECTIVE; i ++)
	{
		tutor_box[i].screen_loc_centre_x = -1;
		tutor_box[i].screen_loc_centre_y = -1;
		tutor_box[i].base_object_recno_no = 0;
		tutor_box[i].pointer_offset_y = -100;
		tutor_box[i].pointer_offset_x = 0;
	}

	selectively_detect_flag = 0;

	bypass_allow_mouse_click = 0;
	bypass_allow_button_click = 0;

	continue_box = 0;
	continue_box_pressed = 0;
	close_box = 0;
	close_box_pressed = 0;
	display_when_view_mode_actived = 0;
	detect_talk_message_id = 0;
	
	//(1) CHECK [allow_mouse_click()]'s selectively_detect_flag

		// selectively_detect_flag has no effect in MENU and OPTION MENU
	
		//*0		= allow detect in non ZOOM and non INFO VIEW
			// always return TRUE


		//*0,5	= allow detect in ZOOM VIEW 
			// always return TRUE	
		// 1,4	= allow detect in ZOOM VIEW 
			//(2) CHECK allow_detect_obj_recno == ClickedLocPtr->base_obj_recno() ||
			//					allow_detect_obj_recno < 0												and
			//(3)	CHECK allow_left_or_right_click == ClickedRightorLeft unless 
			//					allow_left_or_right_click is disabled
			//###### each call to allow_mouse_click() WILL overwrite the followings ######//
			//(4) WRITE detect_obj_recno = locPtr->base_obj_recno();
			//(5) WRITE detect_press_or_release_flag = pressOrRelease;
			//(6) WRITE detect_left_or_right_flag = rightOrLeft;
		
	
		//*0,5	  = allow detect in INFO VIEW 
			// always return TRUE	
		// 2		  = allow detect in INFO VIEW 
			//	CHECK [allow_button_click()]
				//###### if any button is clicked in this objective once, no more can be clicked ######//
				//(7)	 CHECK first allow_detect_button_rc == ClickedButtonRc unless 
				//							allow_detect_button_rc is disabled
				//(8)  CHECK if given allow_detect_button_name == help coed of the clicked button
				// else
				//(9)  CHECK allow_detect_button_x1 bounds the clicked button &&
				//(10) CHECK allow_detect_button_y1 bounds the clicked button &&
				//(11) CHECK allow_detect_button_x2 bounds the clicked button &&
				//(12) CHECK allow_detect_button_y2 bounds the clicked button
				//###### each call to allow_button_click() WILL NOT overwrite the followings ######//
				//###### if any of following variables is setted return 0 ######//
				//(13) WRITE detect_button_name = help coed of the clicked button;
				//(14) WRITE detect_button_x1, y1, x2, y2 = clicked button boundary;
				//(15) WRITE detect_button_rc = ClickedButtonRc;	
			
			//	CHECK [allow_unit_icon_click()]
				
				//(16) CHECK not_allow_unit_info_detect_x2, y2 bounds the ClickedUnitIconBitmapX1, Y1
				//(17) CHECK allow_unit_info_detect_rc bounds == ClickedRc unless
				//					allow_unit_info_detect_rc bounds is disabled
		

		//*0,5	  = allow detect in INFO BUTTON VIEW
			// always return TRUE
		// 3,4     = allow detect in INFO BUTTON VIEW
			//	CHECK [allow_button_click()]
				//###### if any button is clicked in this objective once, no more can be clicked ######//
				//(7)	 CHECK first allow_detect_button_rc == ClickedButtonRc unless 
				//							allow_detect_button_rc is disabled
				//(8)  CHECK if given allow_detect_button_name == help coed of the clicked button
				// else
				//(9)  CHECK allow_detect_button_x1 bounds the clicked button &&
				//(10) CHECK allow_detect_button_y1 bounds the clicked button &&
				//(11) CHECK allow_detect_button_x2 bounds the clicked button &&
				//(12) CHECK allow_detect_button_y2 bounds the clicked button
				//###### each call to [allow_button_click()] WILL NOT overwrite the followings ######//
				//###### if any of following variables is setted return 0 ######//
				//(13) WRITE detect_button_name = help coed of the clicked button;
				//(14) WRITE detect_button_x1, y1, x2, y2 = clicked button boundary;
				//(15) WRITE detect_button_rc = ClickedButtonRc;	
		

		//*6 = do not allow any detect in screen


	//CHECK [detect_button()]
	
		//(15) CHECK detect_button_rc == RequiredButtonRc unless 
		//						detect_button_rc is disabled
		//(13) CHECK if given detect_button_name == RequiredButtonName
		// else
		//(14) CHECK detect_button_x1, y1, x2, y2 bounds the RequiredButtonBoundary
		//###### each call to [detect_button()] WILL reset the followings ######//
		//###### unless the above condition is true ######//
		//(13) WRITE detect_button_name = "";
		//(14) WRITE detect_button_x1, y1, x2, y2 = 0;
		//(15) WRITE detect_button_rc = 0;	
	}
// ------- End of function Tutor::reset_detect_variables -----//

// even using the following function, there may still got problem as 
// checking occurs each day not each frame	
// ------- Begin of function Tutor::custom_detect_is_unit_selected -----//
int Tutor::custom_detect_is_unit_selected(int unitRecno)
{
	// variable may update within one day
	if (unit_array.selected_recno == unitRecno &&
		unit_array.selected_count == 1)
		return 1;
	return 0;
}
// ------- End of function Tutor::custom_detect_is_unit_selected -----//


// ------- Begin of function Tutor::custom_detect_is_town_selected -----//
int Tutor::custom_detect_is_town_selected(int townRecno)
{
	// variable may update within one day
	if (town_array.selected_recno == townRecno)
		return 1;
	return 0;
}
// ------- End of function Tutor::custom_detect_is_town_selected -----//

// ------- Begin of function Tutor::custom_detect_is_firm_selected -----//
// int linkedTownNationRecno = -1 when ignore
//
int Tutor::custom_detect_is_firm_selected(int firmRecno, int linkedTownNationRecno)
{
	// variable may update within one day
	if (firm_array.selected_recno == firmRecno && !firm_array.is_deleted(firm_array.selected_recno))
	{
		if (linkedTownNationRecno < 0)
			return 1;

		Firm *firmPtr = firm_array[firm_array.selected_recno];
		for(int i=firmPtr->linked_town_count-1; i>=0; i--)
		{
			if( town_array.is_deleted(firmPtr->linked_town_array[i]) )
				continue;
			
			Town *townPtr = town_array[firmPtr->linked_town_array[i]];
			if( townPtr->nation_recno == linkedTownNationRecno )
				return 1;
		}
	}
	return 0;
}
// ------- End of function Tutor::custom_detect_is_firm_selected -----//

// ------- Begin of function Tutor::custom_detect_is_any_unit_returning_to_firm ------- //
int Tutor::custom_detect_is_any_unit_returning_to_firm(int firmRecno)
{
	Unit* unitPtr;

	if (!nation_array.player_recno)
		return 0;

	for( int i=1 ; i<=unit_array.size() ; i++ )
	{
		if( unit_array.is_deleted(i) )
			continue;

		unitPtr = unit_array[i];

		if( unitPtr->nation_recno != (~nation_array)->nation_recno	)
			continue;
		
		if( unitPtr->home_camp_firm_recno != firmRecno )
			continue;

		if( unitPtr->cur_order.mode != UNIT_ASSIGN )
			continue;

		if( firm_array.is_deleted(firmRecno) )
			continue;

		Firm* firmPtr = firm_array[firmRecno];
		if( unitPtr->cur_order.para != firmPtr->base_obj_recno)
			continue;

		if( unitPtr->cur_order.loc_x != firmPtr->loc_x1)
			continue;

		if( unitPtr->cur_order.loc_y != firmPtr->loc_y1)
			continue;

		return 1;
	}

	return 0;
}
// ------- End of function Tutor::custom_detect_is_any_unit_returning_to_firm ------- //

// ------- Begin of function Tutor::custom_detect_is_any_unit_returning_to_town ------- //
int Tutor::custom_detect_is_any_unit_returning_to_town(int townRecno)
{
	Unit* unitPtr;

	if (!nation_array.player_recno)
		return 0;

	for( int i=1 ; i<=unit_array.size() ; i++ )
	{
		if( unit_array.is_deleted(i) )
			continue;

		unitPtr = unit_array[i];

		if( unitPtr->nation_recno != (~nation_array)->nation_recno	)
			continue;
		
		if( unitPtr->cur_order.mode != UNIT_ASSIGN )
			continue;

		if( town_array.is_deleted(townRecno) )
			continue;

		Town* townPtr = town_array[townRecno];
		if( unitPtr->cur_order.para != townPtr->base_obj_recno)
			continue;

		if( unitPtr->cur_order.loc_x != townPtr->loc_x1)
			continue;

		if( unitPtr->cur_order.loc_y != townPtr->loc_y1)
			continue;

		return 1;
	}
	
	return 0;
}
// ------- End of function Tutor::custom_detect_is_any_unit_returning_to_town ------- //

// ------- Begin of function Tutor::custom_detect_is_any_unit_attacking_firm ------- //
int Tutor::custom_detect_is_any_unit_attacking_firm(int firmRecno)
{
	Unit* unitPtr;

	if (!nation_array.player_recno)
		return 0;

	for( int i=1 ; i<=unit_array.size() ; i++ )
	{
		if( unit_array.is_deleted(i) )
			continue;

		unitPtr = unit_array[i];

		if( unitPtr->nation_recno != (~nation_array)->nation_recno	)
			continue;
		
		if( unitPtr->cur_order.mode != UNIT_ATTACK )
			continue;

		if( firm_array.is_deleted(firmRecno) )
			continue;

		Firm* firmPtr = firm_array[firmRecno];
		if( unitPtr->cur_order.para != firmPtr->base_obj_recno)
			continue;

		if( unitPtr->cur_order.loc_x != firmPtr->loc_x1)
			continue;

		if( unitPtr->cur_order.loc_y != firmPtr->loc_y1)
			continue;

		return 1;
	}
	
	return 0;
}
// ------- End of function Tutor::custom_detect_is_any_unit_attacking_firm ------- //

// ------- Begin of function Tutor::custom_detect_is_any_unit_attacking_town ------- //
int Tutor::custom_detect_is_any_unit_attacking_town(int townRecno)
{
	Unit* unitPtr;

	if (!nation_array.player_recno)
		return 0;

	for( int i=1 ; i<=unit_array.size() ; i++ )
	{
		if( unit_array.is_deleted(i) )
			continue;

		unitPtr = unit_array[i];

		if( unitPtr->nation_recno != (~nation_array)->nation_recno	)
			continue;
		
		if( unitPtr->cur_order.mode != UNIT_ATTACK )
			continue;

		if( town_array.is_deleted(townRecno) )
			continue;

		Town* townPtr = town_array[townRecno];
		if( unitPtr->cur_order.para != townPtr->base_obj_recno)
			continue;

		if( unitPtr->cur_order.loc_x != townPtr->loc_x1)
			continue;

		if( unitPtr->cur_order.loc_y != townPtr->loc_y1)
			continue;

		return 1;
	}
	
	return 0;
}
// ------- End of function Tutor::custom_detect_is_any_unit_attacking_town ------- //


// ------- Begin of function Tutor::custom_detect_is_any_unit_selected ------- //
int Tutor::custom_detect_is_any_unit_selected(int unitRecno, char orderMode, char visible, int rankId, int isCivilian, int isSpy, int onlyOneSelected)
{
	Unit* unitPtr;

	if (onlyOneSelected && unit_array.selected_count > 1)
		return 0;

	if (!nation_array.player_recno)
		return 0;

	if (unitRecno > 0)
	{
		if( unit_array.is_deleted(unitRecno) )
			return 0;

		unitPtr = unit_array[unitRecno];

		if( unitPtr->nation_recno != (~nation_array)->nation_recno	)
			return 0;

		if( !unitPtr->selected_flag )
			return 0;
		
		if( orderMode >= 0 && unitPtr->cur_order.mode != orderMode )
			return 0;

		if( isCivilian >= 0 && unitPtr->is_civilian() != isCivilian )
			return 0;
		
		if( rankId >= 0 && unitPtr->rank_id != rankId )
			return 0;

		if( isSpy >= 0 && unitPtr->is_own_spy() != isSpy )
			return 0;

		if( unitPtr->is_visible() != visible  )
			return 0;

		return 1;
	}

	for( int i=1 ; i<=unit_array.size() ; i++ )
	{
		if( unit_array.is_deleted(i) )
			continue;

		unitPtr = unit_array[i];

		if( unitPtr->nation_recno != (~nation_array)->nation_recno	)
			continue;

		if( !unitPtr->selected_flag )
			continue;
				
		if( orderMode >= 0 && unitPtr->cur_order.mode != orderMode )
			continue;

		if( isCivilian >= 0 && unitPtr->is_civilian() != isCivilian )
			continue;

		if( rankId >= 0 && unitPtr->rank_id != rankId )
			continue;

		if( isSpy >= 0 && unitPtr->is_own_spy() != isSpy )
			continue;

		if( unitPtr->is_visible() != visible )
			continue;
		
		return 1;
	}
	
	return 0;
}
// ------- End of function Tutor::custom_detect_is_any_unit_selected ------- //

// ------- Begin of function Tutor::custom_search_independent_town ------- //
int Tutor::custom_search_independent_town(int startTownRecno, char raceId, 
						int popUpLimit, int popLoLimit, int resisUpLimit, int resisLoLimit)
{
	if (startTownRecno < 1)
		startTownRecno = 1;

	for( int townRecno = startTownRecno; townRecno <= town_array.size(); townRecno++ )
	{
		if( town_array.is_deleted(townRecno) )
			continue;

		Town *townPtr = town_array[townRecno];
	//	if( townPtr->nation_recno != (~nation_array)->nation_recno	)
	//		continue;

		if( townPtr->nation_recno != 0 )
			continue;

		if( townPtr->under_construction )
			continue;

		if( townPtr->race_id != raceId)
			continue;

		if( townPtr->population > popUpLimit || townPtr->population < popLoLimit )
			continue;

		if (nation_array.player_recno)
			if( townPtr->resistance(nation_array.player_recno) > resisUpLimit || 
				townPtr->resistance(nation_array.player_recno) < resisLoLimit )
				continue;

		return townRecno;
	}
		
	return -1;	
}
// ------- End of function Tutor::custom_search_independent_town ------- //

// ------- Begin of function Tutor::custom_detect_is_any_town_selected ------- //
int Tutor::custom_detect_is_any_town_selected()
{
	if( town_array.is_deleted(town_array.selected_recno) )
		return 0;

	if (!nation_array.player_recno)
		return 0;

	Town *townPtr = town_array[town_array.selected_recno];
	if( townPtr->nation_recno == (~nation_array)->nation_recno )
		return 1;

	return 0;	
}
// ------- End of function Tutor::custom_detect_is_any_town_selected ------- //

// ------- Begin of function Tutor::custom_search_built_firm ------- //
// int linkedTownNationRecno = -1 when ignore
//										  
int Tutor::custom_search_built_firm(int firmId, int linkedTownNationRecno, int checkWorker)
{
	int firmCount = 0;

	if (!nation_array.player_recno)
		return firmCount;

	for( int firmRecno = firm_array.size(); firmRecno > 0; --firmRecno )
	{
		if( firm_array.is_deleted(firmRecno) )
			continue;

		Firm *firmPtr = firm_array[firmRecno];
		if( firmPtr->nation_recno != (~nation_array)->nation_recno	)
			continue;
		
		if( firmPtr->under_construction )
			continue;

		if( checkWorker )
		{
			if( !firmPtr->cast_to_FirmWork() || firmPtr->cast_to_FirmWork()->worker_count < 1 )
				continue;
		}
		
		if( firmPtr->firm_id == firmId )
		{
			if (linkedTownNationRecno < 0)
				firmCount ++;

			for(int i=firmPtr->linked_town_count-1; i>=0; i--)
			{
				if( town_array.is_deleted(firmPtr->linked_town_array[i]) )
					continue;
			
				Town *townPtr = town_array[firmPtr->linked_town_array[i]];
				if( townPtr->nation_recno == linkedTownNationRecno )
					firmCount ++;
			}
		}

	}

	return firmCount;
}
// ------- End of function Tutor::custom_search_built_firm ------- //

// ------- Begin of function Tutor::custom_detect_is_firm_type_selected ------- //
// int linkedTownNationRecno = -1 when ignore
//										  
int Tutor::custom_detect_is_firm_type_selected(int firmId, int linkedTownNationRecno)
{
	if( firm_array.is_deleted(firm_array.selected_recno) )
		return 0;

	if (!nation_array.player_recno)
		return 0;

	Firm *firmPtr = firm_array[firm_array.selected_recno];
	if( firmPtr->nation_recno == (~nation_array)->nation_recno &&
		firmPtr->firm_id == firmId )
	{
		if (linkedTownNationRecno < 0)
			return 1;

		for(int i=firmPtr->linked_town_count-1; i>=0; i--)
		{
			if( town_array.is_deleted(firmPtr->linked_town_array[i]) )
				continue;
			
			Town *townPtr = town_array[firmPtr->linked_town_array[i]];
			if( townPtr->nation_recno == linkedTownNationRecno )
				return 1;
		}
	}
	return 0;	
}
// ------- End of function Tutor::custom_detect_is_firm_type_selected ------- //

// ------- Begin of function Tutor::custom_detect_is_unit_type_selected ------- //
int Tutor::custom_detect_is_unit_type_selected(int unitId)
{
	if( unit_array.is_deleted(unit_array.selected_recno) ||
		unit_array.selected_count > 1)
		return 0;

	if (!nation_array.player_recno)
		return 0;

	Unit *unitPtr = unit_array[unit_array.selected_recno];
	if( unitPtr->nation_recno == (~nation_array)->nation_recno &&
		unitPtr->unit_id == unitId )
		return 1;

	return 0;	
}
// ------- End of function Tutor::custom_detect_is_unit_type_selected ------- //

// ------- Begin of function Tutor::disable_hero_joining ------- //
void Tutor::disable_hero_joining()
{
	for(int i = 1; i <= hero_res.hero_count; ++i )
	{
		HeroInfo *heroInfo = hero_res[i];
		heroInfo->appeared_flag = 1;
	}		
}
// ------- End of function Tutor::disable_hero_joining ------- //

// ------- Begin of function Tutor::disable_message ------- //
void Tutor::disable_message()
{
	message_disable_flag = 1;
}
// ------- End of function Tutor::disable_message ------- //

// ------- Begin of function Tutor::enable_message ------- //
void Tutor::enable_message()
{
	message_disable_flag = 0;
}
// ------- End of function Tutor::enable_message ------- //
	
// ------- Begin of function Tutor::disable_event ------- //
void Tutor::disable_random_event()
{
	config.random_event_frequency = 0;
}
// ------- end of function Tutor::disable_event ------- //

// ------- Begin of function Tutor::set_king_immortal ------- //
void Tutor::set_king_immortal()
{
	config.king_undie_flag = 1;
}
// ------- End of function Tutor::set_king_immortal ------- //

// ------- Begin of function Tutor::set_king_not_immortal ------- //
void Tutor::set_king_not_immortal()
{
	config.king_undie_flag = 0;
}
// ------- End of function Tutor::set_king_not_immortal ------- //

// ------- Begin of function Tutor::set_fast_build ------- //
void Tutor::set_fast_build()
{
	config.fast_build = 1;
}
// ------- End of function Tutor::set_fast_build ------- //

// ------- Begin of function Tutor::disable_fast_build ------- //
void Tutor::disable_fast_build()
{
	config.fast_build = 0;
}
// ------- End of function Tutor::disable_fast_build ------- //

// ------- Begin of function Tutor::set_tech_level_max ------- //
void Tutor::set_tech_level_max(int techLevel, int classId)
{
//	for (int i = 0; i < techLevel; i ++)
//		tech_res.inc_all_tech_level(nation_array.player_recno);

	TechInfo* techInfo = tech_res.tech_info_array;
	for( int i=1 ; i<=tech_res.tech_count ; i++, techInfo++ )
	{
		if (classId < 0 || techInfo->class_id == classId)
		{
			if( techLevel <= techInfo->max_tech_level(nation_array.player_recno) )
				techInfo->set_nation_tech_level( nation_array.player_recno, techLevel );
			else
				techInfo->set_nation_tech_level( nation_array.player_recno, 
					techInfo->max_tech_level(nation_array.player_recno) );
		}
	}
}
// ------- End of function Tutor::set_tech_level_max ------- //

// ------- Begin of function Tutor::disable_report_button_display ------- //
void Tutor::disable_report_button_display()
{
	should_not_display_report_button_flag = 1;
}
// ------- End of function Tutor::disable_report_button_display ------- //

// ------- Begin of function Tutor::enable_report_button_display ------- //
void Tutor::enable_report_button_display()
{
	should_not_display_report_button_flag = 0;
}
// ------- End of function Tutor::enable_report_button_display ------- //

// ------- Begin of function Tutor::custom_detect_view_mode ------- //
int Tutor::custom_detect_view_mode(int viewMode)
{
	if (sys.view_mode == viewMode)
		return 1;

	return 0;
}
// -------- End of function Tutor::custom_detect_view_mode -------- //

// ------- Begin of function Tutor::set_view_mode ------- //
void Tutor::set_view_mode(int viewMode)
{
	sys.set_view_mode(viewMode);
}
// ------- end of function Tutor::set_view_mode ------- //

//------------- Begin of function Tutor::custom_detect_talk_message_id -------------//
//
// <int> talkMessageId - id of just proposed message
//
int Tutor::custom_detect_talk_message_id(int talkMessageId)
{
	if (detect_talk_message_id == talkMessageId)
		return 1;

	return 0;
}
//------------- End of function Tutor::custom_detect_talk_message_id -------------//

//------------ Begin of function Tutor::load_in_game_box -------------//
//
// <int> tutorId - id. of the tutorial
// <int> objectiveRecno - id. of current objective
//
void Tutor::load_in_game_box(int tutorId, int objectiveRecno)
{
//	cur_tutor_id = tutorId;

	//------- get the tutor msgs from the resource file -------//

	int   dataSize;
	char	*in_game_box_text_buf = NULL;
	int	in_game_box_text_buf_size = 0;
	int	in_box_count = 0;

	File* filePtr = res_tutor_text.get_file( tutor[tutorId]->in_box, dataSize);
	if( !filePtr )       // if error getting the tutor resource
	{
		err_here();
		return;
	}

	//------ Open the file and allocate buffer -----//

	FileTxt fileTxt( filePtr, dataSize );  // initialize fileTxt with an existing file stream
	if( dataSize > in_game_box_text_buf_size )
	{
		in_game_box_text_buf      = mem_resize( in_game_box_text_buf, dataSize );       // allocate a buffer larger than we need for the largest size possible
		in_game_box_text_buf_size = dataSize;
	}


	//-------- read in tutor/help text one by one --------//
	char*     textPtr = in_game_box_text_buf;
	int       readLen, totalReadLen=0;    // total length of text read into the tutor_text_buf
	int		 loopCount=0;
	char*		 tokenStr;

	fileTxt.next_line();    // by pass the first two lines of file description
	fileTxt.next_line();

	while( !fileTxt.is_eof() )
	{
		err_when( loopCount++ > 10000 );

		tokenStr = fileTxt.get_token(0);		// don't advance the pointer

		if( !tokenStr )
			break;

		//------ read in the display title of the tutorial segment -------//
	
		int objective_no = 0;
		if( strcmpi( tokenStr, "Title" ) == 0 )
		{
			fileTxt.get_token(1);		// advance the pointer
			while (1)
			{
				tokenStr = fileTxt.get_token(1);
				if ( strcmpi( tokenStr, "*" ) == 0 )
				{
					tokenStr = fileTxt.get_token(1);
					objective_no = (tokenStr[0] - '0') * 10 + tokenStr[1] - '0';
					break;
				}
			}
		}
		
		//------- read in the tutorial text -------//
		readLen = fileTxt.read_paragraph(textPtr, in_game_box_text_buf_size-totalReadLen);

		if (objectiveRecno == objective_no && in_box_count < MAX_IN_GAME_BOX_PER_OBJECTIVE)
		{
			strcpy( tutor_box[in_box_count].text, textPtr );
		//	tutorTextBlock.text_ptr = textPtr;
		//	tutorTextBlock.text_len = readLen;
			in_box_count ++;
		}

		textPtr      += readLen;
		totalReadLen += readLen;

		err_when( totalReadLen > in_game_box_text_buf_size );

		//----------- next tutor block -------------//

		fileTxt.next_line();      // pass the page break line
	}

	if( in_game_box_text_buf )
	{
		mem_del(in_game_box_text_buf);
		in_game_box_text_buf = NULL;
	}
}
//------------- End of function Tutor::load_in_game_box -------------//

//------------- Begin of function Tutor::get_tutorial_type -------------//
//
// <int> campaignId - campaign id of the tutorial
//
int Tutor::get_tutorial_type(int campaignId)
{
//	if ((campaignId%2) == 0)
		return 1;
//	else
//		return 0;
}
//------------- End of function Tutor::get_tutorial_type -------------//


//------------- Begin of function Tutor::allow_this_hot_key -------------//
//
// <int> hotKey - hotkey
//
int Tutor::allow_this_hot_key(int hotKey)
{
	int rc = 0;
	switch(hotKey)
	{
		case KEY_ESC:
         rc = 1;
			break;

		case 'a':		// find artifact on the ground
			if (selectively_detect_flag == 5 || 
				 selectively_detect_flag == 0)				 
				rc = 1;
			break;

		case 'b':		// filter bee and then cycle factory
			// filter bee first
		//	if( filter_unit(filter_unit_id, UNIT_BEE) ) forget this in tutorial, who cares ?
		//	{
		//		// empty
		//	}
		//	else
			if (selectively_detect_flag == 5 || 
				 selectively_detect_flag == 0 ||
				 (selectively_detect_flag == 1 && !(!base_obj_array.is_deleted(allow_detect_obj_recno) && base_obj_array[allow_detect_obj_recno]->obj_type != OBJ_FIRM)))
				rc = 1;
			break;

		case 'B':		// filter bee and then cycle factory
			// filter out bee
         rc = 1;
			break;

		case 'c':		// search caravan
			if (selectively_detect_flag == 5 || 
				 selectively_detect_flag == 0 ||
				 (selectively_detect_flag == 1 && !(!base_obj_array.is_deleted(allow_detect_obj_recno) && base_obj_array[allow_detect_obj_recno]->obj_type != OBJ_UNIT)))
				rc = 1;
			break;

		case 'd':		// search seat of power
			if (selectively_detect_flag == 5 || 
				 selectively_detect_flag == 0 ||
				 (selectively_detect_flag == 1 && !(!base_obj_array.is_deleted(allow_detect_obj_recno) && base_obj_array[allow_detect_obj_recno]->obj_type != OBJ_FIRM)))
				rc = 1;
			break;

		case 'e':		// cycle spy college
			if (selectively_detect_flag == 5 || 
				 selectively_detect_flag == 0 ||
				 (selectively_detect_flag == 1 && !(!base_obj_array.is_deleted(allow_detect_obj_recno) && base_obj_array[allow_detect_obj_recno]->obj_type != OBJ_FIRM)))
				rc = 1;
			break;

		case 'f':		// search camp, fort, lair, grokken fortress
			if (selectively_detect_flag == 5 || 
				 selectively_detect_flag == 0 ||
				 (selectively_detect_flag == 1 && !(!base_obj_array.is_deleted(allow_detect_obj_recno) && base_obj_array[allow_detect_obj_recno]->obj_type != OBJ_FIRM)))
				rc = 1;
			break;

      case 'g':		// search general
         if (selectively_detect_flag == 5 || 
				 selectively_detect_flag == 0 ||
				 (selectively_detect_flag == 1 && !(!base_obj_array.is_deleted(allow_detect_obj_recno) && base_obj_array[allow_detect_obj_recno]->obj_type != OBJ_UNIT)))
				rc = 1;
			break;

		case 'h':		// locate stable
			if (selectively_detect_flag == 5 || 
				 selectively_detect_flag == 0 ||
				 (selectively_detect_flag == 1 && !(!base_obj_array.is_deleted(allow_detect_obj_recno) && base_obj_array[allow_detect_obj_recno]->obj_type != OBJ_FIRM)))
				rc = 1;
			break;

		case 'i':		// locate inn
			if (selectively_detect_flag == 5 || 
				 selectively_detect_flag == 0 ||
				 (selectively_detect_flag == 1 && !(!base_obj_array.is_deleted(allow_detect_obj_recno) && base_obj_array[allow_detect_obj_recno]->obj_type != OBJ_FIRM)))
				rc = 1;
			break;

		case 'j':		// jump to a location with natural resource
         if (selectively_detect_flag == 5 || 
				 selectively_detect_flag == 0)
				rc = 1;
			break;

      case 'k':		// search king
         if (selectively_detect_flag == 5 || 
				 selectively_detect_flag == 0 ||
				 (selectively_detect_flag == 1 && !(!base_obj_array.is_deleted(allow_detect_obj_recno) && base_obj_array[allow_detect_obj_recno]->obj_type != OBJ_UNIT)))
				rc = 1;
			break;

      case 'l':		// load game
			if (selectively_detect_flag == 5 || 
				 selectively_detect_flag == 0 ||
				 selectively_detect_flag == 1)
				rc = 1;
			break;

		case 'm':		// search market
			if (selectively_detect_flag == 5 || 
				 selectively_detect_flag == 0 ||
				 (selectively_detect_flag == 1 && !(!base_obj_array.is_deleted(allow_detect_obj_recno) && base_obj_array[allow_detect_obj_recno]->obj_type != OBJ_FIRM)))
				rc = 1;
			break;

		case 'n':		// search offensive structure magic tor, lishorrs
			if (selectively_detect_flag == 5 || 
				 selectively_detect_flag == 0 ||
				 (selectively_detect_flag == 1 && !(!base_obj_array.is_deleted(allow_detect_obj_recno) && base_obj_array[allow_detect_obj_recno]->obj_type != OBJ_FIRM)))
				rc = 1;
			break;

      case 'o':		// bring up the option menu  
			if (selectively_detect_flag == 5 || 
				 selectively_detect_flag == 0 ||
				 selectively_detect_flag == 1)
				rc = 1;
			break;

      case 'q':	// keys for toggling map mode
			rc = 1;
			break;

		case 'r':
			rc = 0;
			break;

      case 's':	//---- keys for saving and loading game -----//
         if (selectively_detect_flag == 5 || 
				 selectively_detect_flag == 0 ||
				 selectively_detect_flag == 1)
				rc = 1;
			break;

		case 't':		// search tower of science
			if (selectively_detect_flag == 5 || 
				 selectively_detect_flag == 0 ||
				 (selectively_detect_flag == 1 && !(!base_obj_array.is_deleted(allow_detect_obj_recno) && base_obj_array[allow_detect_obj_recno]->obj_type != OBJ_FIRM)))
				rc = 1;
			break;

		case 'u':		// search mine
			if (selectively_detect_flag == 5 || 
				 selectively_detect_flag == 0 ||
				 (selectively_detect_flag == 1 && !(!base_obj_array.is_deleted(allow_detect_obj_recno) && base_obj_array[allow_detect_obj_recno]->obj_type != OBJ_FIRM)))
				rc = 1;
			break;

		case 'v':		// search war factory and fryhtan strcture
			if (selectively_detect_flag == 5 || 
				 selectively_detect_flag == 0 ||
				 (selectively_detect_flag == 1 && !(!base_obj_array.is_deleted(allow_detect_obj_recno) && base_obj_array[allow_detect_obj_recno]->obj_type != OBJ_FIRM)))
				rc = 1;
			break;

		case 'w':		// search wagon
			if (selectively_detect_flag == 5 || 
				 selectively_detect_flag == 0 ||
				 (selectively_detect_flag == 1 && !(!base_obj_array.is_deleted(allow_detect_obj_recno) && base_obj_array[allow_detect_obj_recno]->obj_type != OBJ_UNIT)))
				rc = 1;
			break;

		case 'x':	// clear news messages 
			rc = 1;
			break;

      case 'y':	// locate spy
         if (selectively_detect_flag == 5 || 
				 selectively_detect_flag == 0 ||
				 (selectively_detect_flag == 1 && !(!base_obj_array.is_deleted(allow_detect_obj_recno) && base_obj_array[allow_detect_obj_recno]->obj_type != OBJ_UNIT)))
				rc = 1;
			break;

		case 'z':
			if (selectively_detect_flag == 5 || 
				 selectively_detect_flag == 0 ||
				 (selectively_detect_flag == 1 && !(!base_obj_array.is_deleted(allow_detect_obj_recno) && base_obj_array[allow_detect_obj_recno]->obj_type != OBJ_UNIT)))
				rc = 1;
			break;

		case KEY_TAB:
			rc = 1;
			break;

		case KEY_RETURN:
			rc = 1;
			break;

      case ',':
			rc = 1;
			break;

      case '.':
			rc = 1;
			break;

		case KEY_PGUP:
			rc = 1;
			break;

		case KEY_PGDN:
			rc = 1;
			break;

		case KEY_HOME:
			rc = 1;
			break;

		case KEY_END:
			rc = 1;
			break;
   }
	return rc;
}
//------------- End of function Tutor::allow_this_hot_key -------------//
