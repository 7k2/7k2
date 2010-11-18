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

// Filename    : OCAMPGN3.CPP
// Description : functions on royal units

#include <ocampgn.h>
#include <all.h>
#include <onation.h>
#include <ounit.h>
#include <ospy.h>
#include <ostate.h>
#include <of_camp.h>
#include <ogammenu.h>
#include <otechres.h>
#include <oc_east.h>
#include <osys.h>
#include <ovga.h>
#include <omodeid.h>
#include <ofont.h>
#include <key.h>
#include <omouse.h>
#include <omousecr.h>
#include <ocoltbl.h>
#include <ovgalock.h>
#include <obitmap.h>
#include <oblob.h>
#include <obox.h>
#include <oimgres.h>
#include <ogame.h>
#include <omusic.h>
#include <ot_camp.h>
#include <ot_unit.h>

#define CURRENT_MAX_ROYAL_UNIT 5

//----- Begin of function Campaign::catch_nation_info ------//
//
// Catch nation relationships and god know info from game nations
// to campaign nations.
//
// This is called when a game has finished running. 
//
void Campaign::catch_nation_info()
{
	for( int i=nation_array.size() ; i>0 ; i-- )
	{
		if( nation_array.is_deleted(i) )
			continue;

		Nation* nationPtr = nation_array[i];

		if( !nationPtr->campaign_nation_recno )
			continue;

		if( is_nation_deleted(nationPtr->campaign_nation_recno) )
			continue;

		CampaignNation* cNation = get_nation(nationPtr->campaign_nation_recno);

		//------- copy king name -----------//

		if( nationPtr->king_unit_recno )
			cNation->king_name_id = unit_array[nationPtr->king_unit_recno]->name_id;

		//---- copy know_base_array[MAX_RACE] to CampaignNation ----//

		memcpy( cNation->know_base_array, nationPtr->know_base_array, sizeof(nationPtr->know_base_array) );

		//----------- copy tech info ------------//

		for( int j=1 ; j<=tech_res.tech_count ; j++ )
		{
			cNation->tech_level_array[j-1] 	     = tech_res[j]->nation_tech_level_array[i-1];
			cNation->research_progress_array[j-1] = tech_res[j]->nation_research_progress_array[i-1];
		}

		//--- copy the latest nation relationship to the CampaignNation ---//

		for( j=1 ; j<=nation_array.size() ; j++ )
		{
			if( nation_array.is_deleted(j) || i==j )
				continue;

			//---- translate j (game nation recno) to t (campaign nation recno) ----//

			int t = nation_array[j]->campaign_nation_recno;

			if( !t )
				continue;

			err_when( t<1 || t>MAX_NATION );

			cNation->relation_array[t-1].should_attack = nationPtr->relation_array[j-1].should_attack;
			cNation->relation_array[t-1].trade_treaty  = nationPtr->relation_array[j-1].trade_treaty;
			cNation->relation_array[t-1].status 		 = nationPtr->relation_array[j-1].status;
		}
	}
}
//------- End of function Campaign::catch_nation_info ------//


//----- Begin of function Campaign::fetch_nation_info ------//
//
// Fetch nation relationships and god know info from campaign
// nations to game nations.
//
// This is called before a game is run.
//
void Campaign::fetch_nation_info()
{
	for( int i=nation_array.size() ; i>0 ; i-- )
	{
		if( nation_array.is_deleted(i) )
			continue;

		Nation* nationPtr = nation_array[i];

		if( !nationPtr->campaign_nation_recno )
			continue;

		CampaignNation* cNation = get_nation(nationPtr->campaign_nation_recno);

		//---- copy know_base_array[MAX_RACE] from CampaignNation ----//

		memcpy( nationPtr->know_base_array, cNation->know_base_array, sizeof(nationPtr->know_base_array) );

		//----------- copy tech info ------------//

		for( int j=1 ; j<=tech_res.tech_count ; j++ )
		{
			tech_res[j]->set_nation_tech_level( i, cNation->tech_level_array[j-1] );
			tech_res[j]->nation_research_progress_array[i-1] = cNation->research_progress_array[j-1];
		}

		//--- copy the latest nation relationship to the CampaignNation ---//

		for( j=1 ; j<=nation_array.size() ; j++ )
		{
			if( nation_array.is_deleted(j) || i==j )
				continue;

			//---- translate j (game nation recno) to t (campaign nation recno) ----//

			int t = nation_array[j]->campaign_nation_recno;

			if( !t )
				continue;

			err_when( t<1 || t>MAX_NATION );

			nationPtr->set_relation_should_attack(j, cNation->relation_array[t-1].should_attack, COMMAND_AUTO);
			nationPtr->set_trade_treaty(j, cNation->relation_array[t-1].trade_treaty);
			nationPtr->set_relation_status(j, cNation->relation_array[t-1].status);
		}
	}
}
//------- End of function Campaign::fetch_nation_info ------//


//----- Begin of function Campaign::update_royal_unit ------//
//
// Update from nation_array, unit_array, spy_array
//
void Campaign::update_royal_unit()
{
	CampaignNation *cNation = get_nation(CAMPAIGN_PLAYER_NATION_RECNO);

	const royalMax = sizeof(cNation->noble) / sizeof(cNation->noble[0]);

	// add king first, so drop_extra_royal_menu can manipulate king's item

	if( !unit_array.is_deleted((~nation_array)->king_unit_recno) )
	{
		Unit *kingPtr = unit_array[(~nation_array)->king_unit_recno];
		cNation->king.update_unit( kingPtr, NULL );
	}
	else
	{
		memset( &cNation->king, 0, sizeof(cNation->king) );
	}

	// remove royal unit put into the game

	int i;
	int royalCount = 0;
	for( i = 0; i < cNation->noble_count; ++i )
	{
		if( !cNation->noble[i].sprite_recno )	// retain not deleted royal
		{
			err_when( royalCount >= royalMax );
			if( royalCount != i )			// avoid overwrite itself
				cNation->noble[royalCount] = cNation->noble[i];
			royalCount++;
		}
	}

	// scan royal units in unit_array

	for( i=1; i <= unit_array.size(); ++i )
	{
		if( unit_array.is_deleted(i) )
			continue;

		Unit *unitPtr = unit_array[i];

		if( unitPtr->nation_recno != nation_array.player_recno )	// throw away cloaked spy
			continue;

		if( unitPtr->rank_id == RANK_KING || !unitPtr->is_royal )
			continue;

		// -------- scan if an old royal --------//

		if( royalCount < royalMax )
		{
			Spy *spyPtr;
			if( unitPtr->spy_recno && unitPtr->true_nation_recno() == unitPtr->nation_recno )
				spyPtr = spy_array[unitPtr->spy_recno];
			else
				spyPtr = NULL;

			// drop spy identity if it come from another country

			cNation->noble[royalCount].update_unit(unitPtr, spyPtr );
			royalCount++;
		}
	}

	// scan royal units in firm

	for( i=1; i <= firm_array.size(); ++i )
	{
		if( firm_array.is_deleted(i) )
			continue;

		Firm *firmPtr = firm_array[i];

		if( firmPtr->nation_recno != nation_array.player_recno )
			continue;

		if( firmPtr->cast_to_FirmCamp())
		{
			FirmCamp *campPtr = firmPtr->cast_to_FirmCamp();
			for( int j = 0; j < campPtr->soldier_count; ++j )
			{
				Soldier *soldierPtr = &campPtr->soldier_array[j];
				if( !soldierPtr->is_royal )
					continue;

				Spy *spyPtr;
				if( soldierPtr->spy_recno && soldierPtr->true_nation_recno(i) == firmPtr->nation_recno )
					spyPtr = spy_array[soldierPtr->spy_recno];
				else
					spyPtr = NULL;

				// drop spy identity if it come from another country

				cNation->noble[royalCount].update_unit(soldierPtr, spyPtr, i);
				royalCount++;
			}
		}
	}

	cNation->noble_count = royalCount;
}
//----- End of function Campaign::update_royal_unit ------//


//----- Begin of function CampaignMember::update_unit ------//

void CampaignMember::update_unit(Unit *unitPtr, Spy *spyPtr)
{
	sprite_recno        = 0;
	unit_id             = unitPtr->unit_id;
	rank_id             = unitPtr->rank_id;
	race_id             = unitPtr->race_id;
	// nation_recno     = unitPtr->nation_recno;
	name_id             = unitPtr->name_id;
	hero_id             = unitPtr->hero_id;
	unique_id           = unitPtr->unique_id;

	loyalty             = unitPtr->loyalty;
	target_loyalty      = unitPtr->target_loyalty;
	skill               = unitPtr->skill;
	nation_contribution = unitPtr->nation_contribution;
	total_reward        = unitPtr->total_reward;

	item                = unitPtr->item;

	// spy attribute
	if( !spyPtr )
	{
		is_spy = 0;
		spy_skill = 0;
		spy_loyalty = 0;
		// true_nation_recno = 0;
		cloaked_nation_recno = 0;
	}
	else
	{
		is_spy = 1;
		spy_skill = spyPtr->spy_skill;
		spy_loyalty = spyPtr->spy_loyalty;
		// true_nation_recno = spyPtr->true_nation_recno;
		cloaked_nation_recno = spyPtr->cloaked_nation_recno;
	}

	// update king

	if( unitPtr->rank_id == RANK_KING )
	{
		// BUGHERE : update king name here
	}
}
//----- End of function CampaignMember::update_unit ------//


//----- Begin of function CampaignMember::update_unit ------//

void CampaignMember::update_unit(Soldier *soldierPtr, Spy *spyPtr, int firmRecno)
{
	sprite_recno        = 0;
	unit_id             = soldierPtr->unit_id;
	rank_id             = soldierPtr->rank_id;
	race_id             = soldierPtr->race_id;
	// nation_recno     = soldierPtr->nation_recno;
	name_id             = soldierPtr->name_id;
	hero_id             = soldierPtr->hero_id;
	unique_id           = soldierPtr->unique_id;

	loyalty             = soldierPtr->loyalty;
	target_loyalty      = soldierPtr->target_loyalty(firmRecno);
	skill               = soldierPtr->skill;
	nation_contribution = 0;
	total_reward        = 0;

	item                = soldierPtr->item;

	// spy attribute
	if( !spyPtr )
	{
		is_spy = 0;
		spy_skill = 0;
		spy_loyalty = 0;
		// true_nation_recno = 0;
		cloaked_nation_recno = 0;
	}
	else
	{
		is_spy = 1;
		spy_skill = spyPtr->spy_skill;
		spy_loyalty = spyPtr->spy_loyalty;
		// true_nation_recno = spyPtr->true_nation_recno;
		cloaked_nation_recno = spyPtr->cloaked_nation_recno;
	}
}
//----- End of function CampaignMember::update_unit ------//


// ---- Begin of function CampaignMember::create_unit ------//

int CampaignMember::create_unit(int nationRecno, int xLoc, int yLoc)
{
	int unitRecno = unit_array.add_unit( unit_id, nationRecno, rank_id, loyalty, xLoc, yLoc );

	if( unitRecno )
	{
		Unit *unitPtr =  unit_array[unitRecno];

		unitPtr->set_name( name_id );
		unitPtr->hero_id    = hero_id;
		unitPtr->unique_id  = unique_id;
		unitPtr->skill      = skill;
		unitPtr->nation_contribution = nation_contribution;
		unitPtr->total_reward = total_reward;
		unitPtr->item       = item;

		if( is_spy )
		{
			unitPtr->spy_recno = spy_array.add_spy(unitRecno, spy_skill );
		}
		unitPtr->set_combat_level(-1);

		sprite_recno = unitPtr->sprite_recno;		// mark this unit has been put to the game
	}

	return unitRecno;
}
// ---- End of function CampaignMember::create_unit ------//


// ---- begin of function CampaignMember::unit_name -------//
//
char* CampaignMember::unit_name(int nationRecno, int withTitle, int firstNameOnly)
{
	static String str;

	err_when( !unit_res.init_flag );
	UnitInfo* unitInfo = unit_res[unit_id];

	str = "";

	//------------------------------------//

	if( is_human() && unit_id != UNIT_WAGON )
	{
		err_when( !race_res.init_flag );

		char *baseName;
		if( hero_id )
		{
			baseName = hero_res[hero_id]->name;
		}
		else
		{
			if( rank_id == RANK_KING )		// use the player name
				baseName = game.campaign()->get_nation(nationRecno)->king_name();
			else
				baseName = race_res[race_id]->get_name(name_id, firstNameOnly?1:0);
		}

		if( withTitle && rank_id != RANK_SOLDIER )		// soldier has no title
			str = text_unit.str_unit_titled( race_id, baseName, rank_id );
		else
			str = baseName;
	}
	else if( is_monster() )
	{
		char *baseName;
		if( rank_id == RANK_KING )		// use the player name
			baseName = game.campaign()->get_nation(nationRecno)->king_name();
		else
			baseName = monster_res.get_name(name_id);

		if( withTitle )
			str = text_unit.str_unit_titled( race_id, baseName, rank_id );
		else
			str = baseName;
	}
	else
	{
		str = unitInfo->name;
	}

   return str;
}
// ---- end of function CampaignMember::unit_name -------//



#define DROPTION_PAGE           0x40000000
#define DROPTION_UNIT_INFO      0x00000001
#define DROPTION_UNITS          0x00000008
#define DROPTION_SPARE_ITEM     0x00000010
#define DROPTION_PAGE_NO        0x00000020
#define DROPTION_FRYHTAN_FORCE  0x00000040
#define DROPTION_ALL            0x7fffffff

// ----- Begin of function Campaign::select_royal_menu ------//
//
// mode = 1  - select MAX_NOBLE royals to stay in the game
// selected unit will be rearrange to front, and royalCount updated
//
// mode = 2  - select MAX_ROYAL_UNIT royals to to into the game
// selected unit will set sprite_recno to non-zero
//
// ** King is always selected but not counted as selected
//
void Campaign::select_royal_menu(CampaignMember *king, CampaignMember *royalList, int& royalCount)
{
	const UNIT_ICON_X1 = SCROLL_SHEET_X1+25;
	const UNIT_ICON_Y1 = SCROLL_SHEET_Y1+39;
	const UNIT_ICON_WIDTH = 50;
	const UNIT_ICON_HEIGHT = 100;
	const UNIT_ROW = 10;
	const UNIT_MAX_ROW = 3;
	const UNIT_ICON_OFFSET_X = UNIT_ICON_WIDTH/2;
	const UNIT_ICON_OFFSET_Y = UNIT_ICON_HEIGHT-28;
	const UNIT_INFO_X1 = SCROLL_SHEET_X1+40;
	const UNIT_INFO_Y1 = SCROLL_SHEET_Y2+16;
	const UNIT_INFO_X2 = SCROLL_SHEET_X2-40;
	const UNIT_INFO_Y2 = SCROLL_SHEET_Y2+74;
	const ITEM_OFFSET_X = 27;
	const ITEM_OFFSET_Y = 25;
	const TICK_OFFSET_X = 28;
	const TICK_OFFSET_Y = 2;
	const tickWidth = 20;
	const tickHeight = 20;

//	char *helpText = text_campaign.str_select_royal_help_1(); // "Check the units that you wish to use in this scenario. You may Click on an Artifact and move it to another unit.";
	int selectedRecno = 0;

	int refreshFlag = DROPTION_ALL;
//	int page = 0;
//	int maxPage = (royalCount + UNIT_PAGE - 1) / UNIT_PAGE;	// ceiling
//	if( maxPage <= 0 )
//		maxPage = 1;

	// ------- backup campaign member ------//

	int backupCount = royalCount;
	Blob backupMember( sizeof(CampaignMember) * (royalCount+1) );	// add a bit more
	memcpy( backupMember.p(), royalList, sizeof(CampaignMember) * royalCount);
	CampaignMember backupKing;
	if( king )
		backupKing = *king;
	else
		memset( &backupKing, 0, sizeof(backupKing) );

	// ------ selected flag -------//

	Blob selectFlagBlob( royalCount+4 );		// add a bit more
	char *selectedFlagArray = selectFlagBlob.p();
	memset( selectedFlagArray, 0, royalCount);

	// ------ item slot -------//

	Item freeSlot;

	// ------- temp open resource -------/

	TempGameSet tempGameSet(1);
	TempUnitRes tempUnitRes;
	TempMonsterRes tempMonsterRes;
	TempRaceRes tempRaceRes;
	TempHeroRes tempHeroRes;
	TempItemRes tempItemRes;

	// ------ assume player's nation -----//

	int colorSchemeId = get_nation(CAMPAIGN_PLAYER_NATION_RECNO)->color_scheme_id;

	// --------- select fryhtan force --------//

	//  used to set this_battle_use_fryhtan
	//  check another needSelectFryhtan predicate in this cpp 
	int needSelectFryhtan = can_use_fryhtan && surrendered_fryhtan_id &&
		 (attacker_state_recno && state_array[attacker_state_recno]->campaign_nation_recno == CAMPAIGN_PLAYER_NATION_RECNO ||
		 attacker2_state_recno && state_array[attacker2_state_recno]->campaign_nation_recno == CAMPAIGN_PLAYER_NATION_RECNO);
	int askFryhtanForceFirst = 1;
	//char *humanForceStr = "Human Only";
	//char *fryhtanForceStr = "Human & Fryhtan";

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
					return;
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
				refreshFlag = DROPTION_ALL;
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
				if( refreshFlag & DROPTION_PAGE )
				{
					vga.use_back();
					vga.disp_image_file("CHOOSE");

					// ----- display "Royal Units" --------//

					font_bold_black.center_put(SCROLL_SHEET_X1+10, 81, SCROLL_SHEET_X1+210, 116, 
						text_campaign.str_royal_units() );
						//"Royal Units");
					font_cara.put_paragraph( SCROLL_SHEET_X1+210, 81, SCROLL_SHEET_X2-10, 116, 
						text_campaign.str_select_royal_help_1(), // helpText,
						0, 0 );

					// ----- display frame ---------//

					{
						int frameX1 = UNIT_ICON_X1-5;
						int frameY1 = UNIT_ICON_Y1-5;
						int frameX2 = UNIT_ICON_X1+UNIT_ICON_WIDTH*(UNIT_ROW+1)+5;
						int frameY2 = UNIT_ICON_Y1+UNIT_MAX_ROW*UNIT_ICON_HEIGHT+5;
						vga.active_buf->bar( frameX1, frameY1, frameX2, frameY2, V_BLACK );
						vga.active_buf->draw_d3_up_border( frameX1, frameY1, frameX2, frameY2 );
					}

					// ----- display done, restore button ------//

					font_thin_black.center_put( BUTTON2_X1, BUTTON2_Y1, BUTTON2_X2, BUTTON2_Y2,
						text_campaign.str_done() ); // "Done" );
					font_thin_black.center_put_paragraph( BUTTON4_X1, BUTTON4_Y1, BUTTON4_X2, BUTTON4_Y2,
						text_campaign.str_restore() ); // "Restore" );
					font_thin_black.center_put_paragraph( BUTTON7_X1, BUTTON7_Y1, BUTTON7_X2, BUTTON7_Y2,
						text_campaign.str_random_select() ); // "Random Select" );
					font_thin_black.center_put_paragraph( BUTTON8_X1, BUTTON8_Y1, BUTTON8_X2, BUTTON8_Y2,
						text_campaign.str_deselect_all() ); // "Deselect All" );
					font_thin_black.center_put_paragraph( BUTTON9_X1, BUTTON9_Y1, BUTTON9_X2, BUTTON9_Y2,
						text_campaign.str_delete_royal() ); // "Delete" );

					vga.use_front();
					vga.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
				}

				if( refreshFlag & DROPTION_UNITS )
				{
					for( int r = king?0:1; r <= (UNIT_ROW*UNIT_MAX_ROW); ++r )
					{
						int x1, y1, x2, y2;
						CampaignMember *unit = NULL;

						if( r == 0 )		// king slot
						{
							err_when(!king);
							x1 = UNIT_ICON_X1;
							y1 = UNIT_ICON_Y1;
							unit = king;
						}
						else
						{
							x1 = ((r-1)%UNIT_ROW +1) * UNIT_ICON_WIDTH + UNIT_ICON_X1;
							y1 = ((r-1)/UNIT_ROW) * UNIT_ICON_HEIGHT + UNIT_ICON_Y1;
							if( r <= royalCount )
								unit = royalList + r - 1;
						}

						x2 = x1 + UNIT_ICON_WIDTH-1;
						y2 = y1 + UNIT_ICON_HEIGHT-1;

						// ------ clear page from back buffer ------//

						mouse.hide_area( x1, y1, x2, y2 );

						vga.blt_buf( x1, y1, x2, y2, 0 );

						if( unit )
						{
							// ------ display unit info --------//

							// draw unit info
							info.draw_unit_icon( x1+UNIT_ICON_OFFSET_X, y1+UNIT_ICON_OFFSET_Y,
								unit->unit_id, -colorSchemeId, 
								x1, y1, x2, y2, 1 );

							// display item

							if( unit->item.id )
							{
								vga.active_buf->put_bitmap_trans( x1+ITEM_OFFSET_X, y1+ITEM_OFFSET_Y, item_res.item_unit_interface(unit->item.id) );
							}

							// display combat level
							font_whbl.right_put( x1+26, y1+2, m.format(unit->combat_level()) );

							// display leadership
							if( unit->skill_level() > 0 )
							{
								font_whbl.right_put( x1+26, y1+16, m.format(unit->skill_level()) );
							}

							// display spy skill

							if( unit->is_spy )
							{
								font_whbl.right_put( x1+26, y1+30, m.format(unit->spy_skill) );
							}

							// frame if selected

							if( r == 0 || selectedFlagArray[r-1] > 0 )	// king is always selected
							{
								vga.active_buf->put_bitmap_trans_decompress( x1+TICK_OFFSET_X+4, y1+TICK_OFFSET_Y+4, image_button.read("TICK") );
								vga.active_buf->d3_panel_down( x1+TICK_OFFSET_X, y1+TICK_OFFSET_Y, x1+TICK_OFFSET_X+tickWidth-1, y1+TICK_OFFSET_Y+tickHeight-1, 4, 0 );
							}
							else if( selectedFlagArray[r-1] == 0 )
								vga.active_buf->d3_panel_up( x1+TICK_OFFSET_X, y1+TICK_OFFSET_Y, x1+TICK_OFFSET_X+tickWidth-1, y1+TICK_OFFSET_Y+tickHeight-1 );
							else
							{
								vga.active_buf->d3_panel_up( x1+TICK_OFFSET_X, y1+TICK_OFFSET_Y, x1+TICK_OFFSET_X+tickWidth-1, y1+TICK_OFFSET_Y+tickHeight-1 );
								font_cara.center_put( x1+TICK_OFFSET_X, y1+TICK_OFFSET_Y, x1+TICK_OFFSET_X+tickWidth-1, y1+TICK_OFFSET_Y+tickHeight-1, "X" );
							}

							if( selectedRecno == r )
							{
								vga.active_buf->rect( x1, y1, x2, y2, 1, VGA_DARK_BLUE );
							}
						}

						mouse.show_area();
					}
				}	// end for r

				// ------ display unit information -------//

				if( refreshFlag & DROPTION_UNIT_INFO )
				{
					vga.active_buf->d3_panel_down( UNIT_INFO_X1, UNIT_INFO_Y1, UNIT_INFO_X2, UNIT_INFO_Y2 );

					if( selectedRecno == 0 && king
						|| selectedRecno > 0 && selectedRecno <= royalCount )
					{
						CampaignMember *unit;
						if( selectedRecno == 0 )
							unit = king;
						else
							unit = royalList + selectedRecno - 1;

						font_cara.put( UNIT_INFO_X1+6, UNIT_INFO_Y1+6, unit_res[unit->unit_id]->name );
						font_cara.put( UNIT_INFO_X1+6, UNIT_INFO_Y1+22, unit->unit_name(CAMPAIGN_PLAYER_NATION_RECNO) );
						int x2 = font_cara.put( UNIT_INFO_X1+6, UNIT_INFO_Y1+38, unit->item.item_name() );
						if( unit->item.id )
						{
							x2 = font_cara.put( x2+8, UNIT_INFO_Y1+38, "(" );
							x2 = font_cara.put( x2+2, UNIT_INFO_Y1+38, unit->item.item_desc() );
							x2 = font_cara.put( x2+2, UNIT_INFO_Y1+38, ")" );
						}

						font_cara.put( UNIT_INFO_X1+300, UNIT_INFO_Y1+6, text_unit.str_combat_level() ); // "Combat Level" );
						font_cara.put( UNIT_INFO_X1+450, UNIT_INFO_Y1+6, unit->combat_level() );

						if( unit->skill_level() > 0 )
						{
							font_cara.put( UNIT_INFO_X1+300, UNIT_INFO_Y1+22, text_unit.str_leadership() ); //"Leadership" );
							font_cara.put( UNIT_INFO_X1+450, UNIT_INFO_Y1+22, unit->skill_level() );
						}
						if( unit->is_spy )
						{
							font_cara.put( UNIT_INFO_X1+300, UNIT_INFO_Y1+38, text_unit.str_spy_skill() ); // "Spying" );
							font_cara.put( UNIT_INFO_X1+450, UNIT_INFO_Y1+38, unit->spy_skill );
						}
					}
				}

				if( refreshFlag & DROPTION_FRYHTAN_FORCE )
				{
					if( needSelectFryhtan )
					{
						vga.blt_buf( BUTTON3_X1, BUTTON3_Y1, BUTTON3_X2, BUTTON3_Y2, 0 );
						font_thin_black.center_put_paragraph( BUTTON3_X1+1, BUTTON3_Y1, BUTTON3_X2, BUTTON3_Y2,
							this_battle_use_fryhtan ? text_campaign.str_fryhtan_force() : text_campaign.str_human_force() );
						// fryhtanForceStr : humanForceStr );
					}
				}

				refreshFlag = 0;
			}

			sys.blt_virtual_buf();

			// ------ detect ---------//

			// ask use fryhtan force

			if( needSelectFryhtan 
				&& (askFryhtanForceFirst
				|| mouse.single_click(BUTTON3_X1, BUTTON3_Y1, BUTTON3_X2, BUTTON3_Y2)) )
			{
				//--- if the player has a fryhtan troop to use and the player is an attacker in the next battle, then allow the player to select the type of troop to send. ---//

				this_battle_use_fryhtan = !box.ask( 
					// "Choose the type of force that you wish to field in the next mission:", humanForceStr, fryhtanForceStr );		// use ! to reverse the order
					text_campaign.str_ask_fryhtan(),
					text_campaign.str_human_force(),
					text_campaign.str_fryhtan_force() );	// use ! to reverse the order
				refreshFlag |= DROPTION_FRYHTAN_FORCE;

				if( askFryhtanForceFirst )
				{
					askFryhtanForceFirst = 0;
					continue;	// skip other mouse click, especially when askFryhtanForceFirst is true
				}
			}

			// detect unit area

			for( int r = king?0:1 ; r <= royalCount; ++r )
			{
				int x1, y1, x2, y2;
				CampaignMember *unit;

				if( r == 0 )		// king slot
				{
					err_when(!king);
					x1 = UNIT_ICON_X1;
					y1 = UNIT_ICON_Y1;
					unit = king;
				}
				else
				{
					x1 = ((r-1)%UNIT_ROW +1) * UNIT_ICON_WIDTH + UNIT_ICON_X1;
					y1 = ((r-1)/UNIT_ROW) * UNIT_ICON_HEIGHT + UNIT_ICON_Y1;
					unit = royalList + r - 1;
				}

				x2 = x1 + UNIT_ICON_WIDTH-1;
				y2 = y1 + UNIT_ICON_HEIGHT-1;

				// click at the tick or cross
				if( r > 0 && mouse.any_click(x1+TICK_OFFSET_X, y1+TICK_OFFSET_Y, x1+TICK_OFFSET_X+tickWidth-1, y1+TICK_OFFSET_Y+tickHeight-1) )
				{
					if( ++selectedFlagArray[r-1] > 1)
						selectedFlagArray[r-1] = 0;

					refreshFlag |= DROPTION_UNITS | DROPTION_UNIT_INFO;
				}

				// click at item area
				else if( mouse.any_click(x1+ITEM_OFFSET_X, y1+ITEM_OFFSET_X,
					x1+ITEM_OFFSET_X+ITEM_UNIT_ICON_WIDTH-1, y1+ITEM_OFFSET_Y+ITEM_UNIT_ICON_HEIGHT-1)
					|| freeSlot.id && mouse.release_click(x1, y1, x2, y2) )
				{
					// swap item width free slot

					Item tempItem = unit->item;
					unit->item = freeSlot;
					freeSlot = tempItem;

					// set mouse icon

					if( freeSlot.id )
					{
						mouse_cursor.define_custom_icon( item_res.item_unit_interface(freeSlot.id), 0, 0 );
						mouse_cursor.set_icon(CURSOR_CUSTOM);
					}
					else
					{
						mouse_cursor.set_icon(CURSOR_NORMAL);
					}

					refreshFlag |= DROPTION_UNITS | DROPTION_UNIT_INFO;
				}
				// right click at item area
				else if( unit->item.id && mouse.any_click(x1+ITEM_OFFSET_X, y1+ITEM_OFFSET_X,
					x1+ITEM_OFFSET_X+ITEM_UNIT_ICON_WIDTH-1, y1+ITEM_OFFSET_Y+ITEM_UNIT_ICON_HEIGHT-1, 1) )
				{
					// delete item

					unit->item.clear();
					refreshFlag |= DROPTION_UNITS | DROPTION_UNIT_INFO;
				}

				// click at the unit icon
				if( mouse.any_click(x1, y1, x2, y2) )
				{
					selectedRecno = r;
					refreshFlag |= DROPTION_UNITS | DROPTION_UNIT_INFO;
				}
			}

			// detect restore

			if( mouse.single_click(BUTTON4_X1, BUTTON4_Y1, BUTTON4_X2, BUTTON4_Y2) )
			{
				// restore royalCount and royalList
				memcpy( royalList, backupMember.p(), sizeof(CampaignMember) * backupCount );
				royalCount = backupCount;

				// restore king

				if( king )
					*king = backupKing;

				// restore selectedFlag 
				memset( selectedFlagArray, 0, royalCount);

				// clear freeSlot

				freeSlot.clear();
				mouse_cursor.set_icon( CURSOR_NORMAL);

				refreshFlag |= DROPTION_ALL & ~DROPTION_PAGE;
			}

			// detect random select

			if( mouse.any_click( BUTTON7_X1, BUTTON7_Y1, BUTTON7_X2, BUTTON7_Y2) )
			{
				// temporary random seed
				Misc mTemp;
				mTemp.randomize();

				int s = CURRENT_MAX_ROYAL_UNIT;
				for( int i = royalCount; i > 0; --i )
				{
					if( mTemp.random(i) < s )		// similar to m.random(i) < s
					{
						selectedFlagArray[i-1] = 1;
						s--;
					}
					else
					{
						selectedFlagArray[i-1] = 0;
					}
				}
				refreshFlag |= DROPTION_UNITS;
			}

			// detect deselect all

			if( mouse.single_click( BUTTON8_X1, BUTTON8_Y1, BUTTON8_X2, BUTTON8_Y2) )
			{
				for( int i = royalCount; i > 0; --i )
				{
					selectedFlagArray[i-1] = 0;
				}
				refreshFlag |= DROPTION_UNITS;
			}

			// detect delete

			if( selectedRecno > 0 && selectedRecno <= royalCount
				&& (mouse.any_click( BUTTON9_X1, BUTTON9_Y1, BUTTON9_X2, BUTTON9_Y2) 
				|| mouse.key_code == KEY_DEL) )
			{
				m.del_array_rec( royalList, royalCount, sizeof(royalList[0]), selectedRecno );
				m.del_array_rec( selectedFlagArray, royalCount, sizeof(selectedFlagArray[0]), selectedRecno );
				--royalCount;
				if( selectedRecno > royalCount )
					selectedRecno = royalCount;
				refreshFlag |= DROPTION_UNITS | DROPTION_UNIT_INFO;
			}

			// detect finish

			if( mouse.single_click(BUTTON2_X1, BUTTON2_Y1, BUTTON2_X2, BUTTON2_Y2) )
			{
				// check the number of selected royal and number of royal to keep

				int selectedCount = 0;
				int keepCount = 0;

				for( int i = 0; i < royalCount; ++i )
				{
					if( selectedFlagArray[i] >= 0 )
						++keepCount;
					
					if( selectedFlagArray[i] > 0 )
						++selectedCount;
				}

				if( selectedCount > CURRENT_MAX_ROYAL_UNIT )
				{
					// box.msg("You cannot bring more than 5 royal units to scenario");
					box.msg( text_campaign.str_max_royal_on_field(CURRENT_MAX_ROYAL_UNIT) );
				}
				else if( keepCount > MAX_NOBLE )
				{
					// box.msg("You cannot keep more than 20 royal units, delete some of them");
					box.msg( text_campaign.str_max_royal_in_store(MAX_NOBLE) );
				}
				else
				{
					// confirm to clear item

					// if( box.ask( freeSlot.id ? text_campaign.str_confirm_drop_item() "Drop this item?" : "Are you sure?" ) )
					if( box.ask( freeSlot.id ? text_campaign.str_confirm_drop_item() : text_campaign.str_confirm_end_royal()) )
					{
						freeSlot.clear();
						break;
					}
				}
			}
		}	// end while
	} // end of scope of VgaLock

	// pack removed royal and 
	// set sprite recno of selected unit to -1
	int i, j;
	for( i = j = 0; i < royalCount; ++i )
	{
		if( selectedFlagArray[i] >= 0 )
		{
			if( selectedFlagArray[i] > 0)
				royalList[i].sprite_recno = -1;
			if( i != j )		// avoid overwriting itself
				royalList[j] = royalList[i];
			++j;
		}
	}
	
	royalCount = j; // do not use selectedCount as it doesn't count the king

	mouse_cursor.set_icon(CURSOR_NORMAL);

	return;
}
// ----- End of function Campaign::select_royal_menu ------//

// ----- Begin of function Campaign::pick_royal_to_battle ------//
//
// pick 10 hero to join battle, each selected will set sprite_recno to -1
//
void Campaign::pick_royal_to_battle()
{
	CampaignNation *cNation = get_nation( CAMPAIGN_PLAYER_NATION_RECNO );

	if( !cNation )
		return;

	if( stage_id == CampaignEastWest::STAGE_TRAIN_LEADERS )		// this scenario should not allow using royal units otherwise it will ruin the gameplay
		return;

	//  check another needSelectFryhtan predicate in this cpp
	int needSelectFryhtan = can_use_fryhtan && surrendered_fryhtan_id &&
		 (attacker_state_recno && state_array[attacker_state_recno]->campaign_nation_recno == CAMPAIGN_PLAYER_NATION_RECNO ||
		 attacker2_state_recno && state_array[attacker2_state_recno]->campaign_nation_recno == CAMPAIGN_PLAYER_NATION_RECNO);

	if( cNation->noble_count == 0 && !needSelectFryhtan )
		return;

	// ------- call select royal menu ------//

	select_royal_menu(
		cNation->king.unit_id ? &cNation->king : NULL,
		cNation->noble, cNation->noble_count );
}
// ----- End of function Campaign::pick_royal_to_battle ------//



// ----- Begin of function Campaign::unpick_royal_to_battle -------//
//
void Campaign::unpick_royal_to_battle()
{
	CampaignNation *cNation = get_nation( CAMPAIGN_PLAYER_NATION_RECNO );

	if( !cNation || cNation->noble_count == 0 )
		return;

	// ------- copy king, noble to royal list ------//

	cNation->king.sprite_recno = 0;
	for( int i = 0; i < cNation->noble_count; ++i )
	{
		err_when( cNation->noble[i].rank_id == RANK_KING );
		cNation->noble[i].sprite_recno = 0;
	}
}
// ----- End of function Campaign::unpick_royal_to_battle -------//
