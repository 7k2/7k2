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

// Filename    : OSEDIT_R.CPP
// Description : Scenario editor, Diplomacy mode

#include <oseditor.h>
#include <all.h>
#include <omodeid.h>
#include <omouse.h>
#include <ofont.h>
#include <onationa.h>
#include <ovga.h>
#include <ot_sedit.h>
#include <ot_talk.h>

#define X_SPACING    25
#define Y_SPACING    26
#define PANEL_WIDTH  4

#define LABEL_ROW_X1 (INFO_X1+36)
#define LABEL_ROW_Y1 (INFO_Y1+15)
#define LABEL_COL_X1 (INFO_X1+15)
#define LABEL_COL_Y1 (LABEL_ROW_Y1+Y_SPACING)

#define FIELD_X1     (LABEL_ROW_X1)
#define FIELD_Y1     (LABEL_COL_Y1)


// ----- begin of function ScenarioEditor::init_diplomacy_mode -------//
//
void ScenarioEditor::init_diplomacy_mode()
{
}
// ----- end of function ScenarioEditor::init_diplomacy_mode -------//


// ----- begin of function ScenarioEditor::deinit_diplomacy_mode -------//
//
void ScenarioEditor::deinit_diplomacy_mode()
{
}
// ----- end of function ScenarioEditor::deinit_diplomacy_mode -------//


// ----- begin of function ScenarioEditor::disp_diplomacy_main -------//
//
// display interface area
//
void ScenarioEditor::disp_diplomacy_main(int refreshFlag)
{
	// ------- display relation matrix --------//

	int p, n;
	int x, y;

	int yHintText;

	vga.active_buf->d3_panel_down( FIELD_X1-PANEL_WIDTH-4, FIELD_Y1-PANEL_WIDTH-4,
		FIELD_X1+PANEL_WIDTH+MAX_NATION*X_SPACING-4,
		(yHintText = FIELD_Y1+PANEL_WIDTH+MAX_NATION*Y_SPACING-4) );

	// ------- draw label row ---------//

	x = LABEL_ROW_X1;
	y = LABEL_ROW_Y1;
	for( (n = 1); n <= nation_array.size(); ++n, x += X_SPACING )
	{
		if( !nation_array.is_deleted(n) )
			//font_whbl.center_put( x, y, x+X_SPACING, y+Y_SPACING, m.format(n) );
			nation_array[n]->disp_nation_color(x, y );
	}

	y = FIELD_Y1;
	int labelY = LABEL_COL_Y1;

	String str;
	for( p = 1; p <= nation_array.size(); ++p, (y+= Y_SPACING), (labelY += Y_SPACING) )
	{
		// put nation p

		if( nation_array.is_deleted(p) )
			continue;

		// font_whbl.put( x0, y, p );
		nation_array[p]->disp_nation_color(LABEL_COL_X1, labelY );

		x = FIELD_X1;

		for( n = 1; n <= nation_array.size(); ++n, x += X_SPACING )
		{
			if( nation_array.is_deleted(n) || p == n )
				continue;

			NationRelation *nationRelation = nation_array[p]->get_relation(n);
			str = text_editor.str_abbrev_relation_str( nationRelation->status );
			if( nationRelation->trade_treaty )
				str += text_editor.str_abbrev_trade_treaty();
			font_snds.put( x, y, str );		// first char of the status_str
		}
	}

	// put description of each status
	int col = 0;
	x = INFO_X1+8;
	y = yHintText + 6;
	for( p = RELATION_HOSTILE; p <= RELATION_ALLIANCE; ++p )
	{
		// charStr[0] = text_talk.str_relation_status(p)[0];
		font_zoom.put( x, y, text_editor.str_abbrev_relation_str(p) );
		font_zoom.put( x+17, y, text_talk.str_relation_status(p), 0, MIN(x+108,INFO_X2-5) );
		if( col < 1 )		// 2 column
		{
			x += 110;
			++col;
		}
		else
		{
			y += font_zoom.max_font_height;
			x = INFO_X1+10;
			col = 0;
		}
	}

	// put trade treaty status
	{
		font_zoom.put( x, y, text_editor.str_abbrev_trade_treaty() );
		font_zoom.put( x+17, y, text_talk.str_trade_treaty(), 0, MIN(x+108,INFO_X2-5) );
	}

	// display instruction 
	y += font_zoom.max_font_height + 2;
	font_zoom.put_paragraph( INFO_X1+10, y, INFO_X2-10, INFO_Y2-14, 
		text_editor.str_relation_inst(), 0 );
}
// ----- end of function ScenarioEditor::disp_diplomacy_main -------//


// ----- begin of function ScenarioEditor::detect_diplomacy_main -------//
//
// detect interface area
//
void ScenarioEditor::detect_diplomacy_main()
{
	// ------- display relation matrix --------//

	int m, n;
	int x, y;

	// ------- draw label row ---------//

	y = FIELD_Y1;

	for( m = 1; m <= nation_array.size(); ++m, y+= Y_SPACING )
	{
		// put nation m

		if( nation_array.is_deleted(m) )
			continue;

		Nation *nation = nation_array[m];

		x = FIELD_X1;

		for( n = 1; n <= nation_array.size(); ++n, x += X_SPACING )
		{
			if( nation_array.is_deleted(n) || m == n )
				continue;

			NationRelation *nationRelation = nation->get_relation(n);

			if( mouse.any_click( x, y, x+X_SPACING-1, y+Y_SPACING-1, 0) )
			{
				// left click, promote relation

				if( nationRelation->status > RELATION_HOSTILE && !nationRelation->trade_treaty )
				{
					// promote to trade treaty
					nation->set_trade_treaty(n, 1);
				}
				else if( nationRelation->status < RELATION_ALLIANCE )
				{
					// disable trade treaty and promote to next relation
					nation->set_trade_treaty(n, 0);

					// change last_change_status_date to very old
					// when change from RELATION_TENSE to RELATION_HOSTILE
					// it check last_change_status_date
					{
						nationRelation->last_change_status_date
							= nation_array[n]->get_relation(m)->last_change_status_date
							= info.game_date - 31;
					}
					nation->set_relation_status( n, nationRelation->status+1 );
				}
			}
			else if( mouse.any_click( x, y, x+X_SPACING-1, y+Y_SPACING-1, 1) )
			{
				// right click, demote relation

				if( nationRelation->trade_treaty )
				{
					// demote to trade treaty
					nation->set_trade_treaty(n, 0);
				}
				else if( nationRelation->status > RELATION_HOSTILE )
				{
					// enable trade treaty and demote to previous relation
					nation->set_trade_treaty(n, 1);

					// change last_change_status_date to very old
					// when change from RELATION_TENSE to RELATION_HOSTILE
					// it check last_change_status_date
					{
						nationRelation->last_change_status_date
							= nation_array[n]->get_relation(m)->last_change_status_date
							= info.game_date - 31;
					}
					nation->set_relation_status( n, nationRelation->status-1 );
				}
			}
		}
	}
}
// ----- end of function ScenarioEditor::detect_diplomacy_main -------//


// ----- begin of function ScenarioEditor::disp_diplomacy_view -------//
//
void ScenarioEditor::disp_diplomacy_view()
{
}
// ----- end of function ScenarioEditor::disp_diplomacy_view -------//


// ----- begin of function ScenarioEditor::detect_diplomacy_view -------//
//
int ScenarioEditor::detect_diplomacy_view()
{
	return 0;
}
// ----- end of function ScenarioEditor::detect_diplomacy_view -------//
