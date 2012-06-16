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

//Filename    : OR_TECH.CPP
//Description : Technology Report

#include <oinfo.h>
#include <ovga.h>
#include <vga_util.h>
#include <omodeid.h>
#include <ofont.h>
#include <oimgres.h>
#include <ovbrowif.h>
#include <obutton.h>
#include <oconfig.h>
#include <otechres.h>
#include <otech.h>
#include <ogodres.h>
#include <oraceres.h> 
#include <onation.h>
#include <ou_god.h>
#include <ot_reps.h>
#include <ot_unit.h>

//------------- Define coordinations -----------//

#define TECH_BROWSE_X1 (ZOOM_X1+6)
#define TECH_BROWSE_Y1 (ZOOM_Y1+6)
#define TECH_BROWSE_X2 (ZOOM_X2-6)
#define TECH_BROWSE_Y2 (TECH_BROWSE_Y1+220)

#define SCROLL_X1 (ZOOM_X1+6)
#define SCROLL_Y1 (TECH_BROWSE_Y2+6)
#define SCROLL_X2 (ZOOM_X2-6)
#define SCROLL_Y2 (SCROLL_Y1+120)

#define GOD_BROWSE_X1 (ZOOM_X1+6)
#define GOD_BROWSE_Y1 (SCROLL_Y2+6)
#define GOD_BROWSE_X2 (ZOOM_X2-6)
#define GOD_BROWSE_Y2 (ZOOM_Y2-6)

//----------- Define static variables ----------//

static VBrowseIF browse_tech, browse_god;

//----------- Define static functions ----------//

static int  tech_filter(int recNo=0);
static int  god_filter(int recNo=0);
static void put_tech_rec(int recNo, int x, int y, int refreshFlag);
static void put_god_rec(int recNo, int x, int y, int refreshFlag);
static void disp_owned_scroll();
static void disp_scroll(int x, int y, int raceId);

//--------- Begin of function Info::disp_tech ---------//
//
void Info::disp_tech(int refreshFlag)
{
	//-------- display the technology browser ---------//

	int x=TECH_BROWSE_X1+9;
	int y=TECH_BROWSE_Y1+4;

	vga_back.d3_panel_up(TECH_BROWSE_X1, TECH_BROWSE_Y1, TECH_BROWSE_X2, TECH_BROWSE_Y1+32 );

	font_bld.put( x	 , y+7, text_reports.str_technology() ); // "Technology" );

#if(defined(CHINESE))
	font_bld.center_put_paragraph( x+223, TECH_BROWSE_Y1 +6, x+329, TECH_BROWSE_Y1+32, text_reports.str_tech_present_version(), 0, 1, 0);
	font_bld.center_put_paragraph( x+316, TECH_BROWSE_Y1 +6, x+439, TECH_BROWSE_Y1+32, text_reports.str_tech_researching_version(), 0, 1, 0);
	font_bld.center_put_paragraph( x+413, TECH_BROWSE_Y1 +6, x+540, TECH_BROWSE_Y1+32, text_reports.str_tech_research_progress(), 0, 1, 0);
#else
	// font_bld.center_put_paragraph( x+240, TECH_BROWSE_Y1, x+329, TECH_BROWSE_Y1+32, "Present Version", 0, 1, 0);
	font_bld.center_put_paragraph( x+240, TECH_BROWSE_Y1, x+329, TECH_BROWSE_Y1+32, text_reports.str_tech_present_version(), 0, 1, 0);

	// font_bld.center_put_paragraph( x+330, TECH_BROWSE_Y1, x+439, TECH_BROWSE_Y1+32, "Researching Version", 0, 1, 0);
	font_bld.center_put_paragraph( x+330, TECH_BROWSE_Y1, x+439, TECH_BROWSE_Y1+32, text_reports.str_tech_researching_version(), 0, 1, 0);

	// font_bld.center_put_paragraph( x+440, TECH_BROWSE_Y1, x+510, TECH_BROWSE_Y1+32, "Research Progress", 0, 1, 0);
	// US font_bld.center_put_paragraph( x+440, TECH_BROWSE_Y1, x+510, TECH_BROWSE_Y1+32, text_reports.str_tech_research_progress(), 0, 1, 0);
	font_bld.center_put_paragraph( x+440, TECH_BROWSE_Y1, x+540, TECH_BROWSE_Y1+32, text_reports.str_tech_research_progress(), 0, 1, 0);
#endif

	if( refreshFlag == INFO_REPAINT )
	{
		browse_tech.init( TECH_BROWSE_X1, TECH_BROWSE_Y1+34, TECH_BROWSE_X2, TECH_BROWSE_Y2,
								0, 22, tech_filter(), put_tech_rec, 1 );

		browse_tech.open(browse_tech_recno);
	}
	else
	{
		browse_tech.paint();
		browse_tech.open(browse_tech_recno, tech_filter());
	}

	//----- display the list of acquired scrolls of power ----//

	disp_owned_scroll();

	//-------- display the god unit browser ---------//

	x=GOD_BROWSE_X1+9;
	y=GOD_BROWSE_Y1+4;

	vga_back.d3_panel_up(GOD_BROWSE_X1, GOD_BROWSE_Y1, GOD_BROWSE_X2, GOD_BROWSE_Y1+20 );

	font_bld.put( x	 , y, text_reports.str_tech_greater_being() );  // "Greater Being" );
	font_bld.put( x+300, y, text_unit.str_hit_points() );  // "Hit Points" );

	if( refreshFlag == INFO_REPAINT )
	{
		browse_god.init( GOD_BROWSE_X1, GOD_BROWSE_Y1+22, GOD_BROWSE_X2, GOD_BROWSE_Y2,
							  0, 22, god_filter(), put_god_rec, 1 );

		browse_god.open(browse_god_recno);
	}
	else
	{
		browse_god.paint();
		browse_god.open(browse_god_recno, god_filter());
	}
}
//----------- End of function Info::disp_tech -----------//


//--------- Begin of function Info::detect_tech ---------//
//
void Info::detect_tech()
{
	if( browse_tech.detect() )
		browse_tech_recno = browse_tech.recno();

	if( browse_god.detect() )
	{
		browse_god_recno = browse_god.recno();

		if( browse_god.double_click )
		{
			Unit* unitPtr = unit_array[ god_filter(browse_god.recno()) ];

			world.go_loc( unitPtr->next_x_loc(), unitPtr->next_y_loc(), 1 );
		}
	}
}
//----------- End of function Info::detect_tech -----------//


//-------- Begin of static function tech_filter --------//
//
// This function has dual purpose :
//
// 1. when <int> recNo is not given :
//    - return the total no. of firms of this nation
//
// 2. when <int> recNo is given :
//    - return the firm recno in firm_array of the given recno.
//
static int tech_filter(int recNo)
{
	int techCount=0;
	Nation* nationPtr = nation_array[info.viewing_nation_recno];

	for( int techId=tech_res.tech_count ; techId>0 ; techId-- )
	{
		//--- if spy methodologies are always available, don't show them in the report ---//

		if( tech_res[techId]->class_id == TECH_CLASS_SPYING &&
			 config.spy_methodology )
		{
			continue;
		}

		//---------------------------------------//

		if( (nationPtr->research_tech_id == techId && nationPtr->ai_research_count > 0)
			 || tech_res[techId]->get_nation_tech_level(info.viewing_nation_recno) > 0 )
		{
			techCount++;
		}

		if( recNo && techCount==recNo )
			return techId;
	}

	err_when( recNo );   // the recNo is not found, it is out of range

	return techCount;
}
//----------- End of static function tech_filter -----------//


//-------- Begin of static function god_filter --------//
//
// This function has dual purpose :
//
// 1. when <int> recNo is not given :
//    - return the total no. of firms of this nation
//
// 2. when <int> recNo is given :
//    - return the firm recno in firm_array of the given recno.
//
static int god_filter(int recNo)
{
	int 	godCount=0, totalUnit=unit_array.size();
	Unit* unitPtr;

	for( int i=1 ; i<=totalUnit ; i++ )
	{
		if( unit_array.is_deleted(i) )
			continue;

		unitPtr = unit_array[i];

		if( unitPtr->nation_recno == info.viewing_nation_recno &&
			 unit_res[ unitPtr->unit_id ]->unit_class == UNIT_CLASS_GOD )
		{
			godCount++;
		}

		if( recNo && godCount==recNo )
			return i;
	}

	err_when( recNo );   // the recNo is not found, it is out of range

	return godCount;
}
//----------- End of static function god_filter -----------//


//-------- Begin of static function put_tech_rec --------//
//
static void put_tech_rec(int recNo, int x, int y, int refreshFlag)
{
	int   	 techId   = tech_filter(recNo);
	TechInfo* techInfo = tech_res[techId];

	//---------- display bitmap ----------//

	x+=3;
	y+=3;

	//----------- display info ----------//

	y+=2;

	int curLevel = techInfo->get_nation_tech_level(info.viewing_nation_recno);

	String str;

	switch( techInfo->class_id )
	{
		case TECH_CLASS_SPYING:
			str = text_reports.str_tech_espionage_(); // "Espionage: ";
			break;
	}

	str += techInfo->tech_des();

	font_bld.put( x , y, str );

	if( curLevel >= 1 && techInfo->max_tech_level(0) > 1 )		// only need to display version for tech with more than one level
		font_bld.put( x+275, y, m.roman_number(curLevel) );

	//----- if the nation is researching this technology -----//

	Nation* nationPtr = nation_array[info.viewing_nation_recno];

	if( nationPtr->research_tech_id == techId )
	{
		err_when( curLevel >= techInfo->max_tech_level(info.viewing_nation_recno) );

		if( techInfo->max_tech_level(0) > 1 )		// only need to display version for tech with more than one level
			font_bld.put( x+375, y, m.roman_number(curLevel+1) );

		//----- display the research progress bar -----//

		vga_util.d3_panel_down( x+440, y-2, x+510, y+14 );
		// ######## begin Gilbert 17/3 #########//
		vga_back.indicator( x+441, y-1, x+509, y+13,
			nation_array[info.viewing_nation_recno]->research_progress(), (float)100, VGA_LIGHT_BLUE, V_BLACK );
		// ######## end Gilbert 17/3 #########//
	}
}
//----------- End of static function put_tech_rec -----------//


//-------- Begin of static function put_god_rec --------//
//
static void put_god_rec(int recNo, int x, int y, int refreshFlag)
{
	UnitGod* unitGod = (UnitGod*) unit_array[ god_filter(recNo) ];

	err_when( unit_res[unitGod->unit_id]->unit_class != UNIT_CLASS_GOD );

	//----------- display info ----------//

	x+=3;
	y+=5;

	font_bld.put( x, y, unit_res[unitGod->unit_id]->name );

	//--------- display hit points -----------//

	String str;

	str  = (int) unitGod->hit_points;
	str += "/";
	str += unitGod->max_hit_points();

	font_bld.put( x+300, y, str );
}
//----------- End of static function put_god_rec -----------//


//-------- Begin of static function disp_scroll --------//
//
static void disp_scroll(int x, int y, int raceId)
{
	char iconName[]="SCROLLxx";
	iconName[6] = race_res[raceId]->code[0];
	iconName[7] = race_res[raceId]->code[1];
	char *bmpPtr = image_spict.get_ptr(iconName);
	if( !bmpPtr )                                 //BUGHERE, the icons are not complete yet
		err_here();

	image_spict.put_back( x, y, iconName );
}
//----------- End of static function disp_scroll -----------//


//-------- Begin of static function disp_owned_scroll --------//
//
static void disp_owned_scroll()
{
	vga_util.d3_panel_down( SCROLL_X1, SCROLL_Y1, SCROLL_X2, SCROLL_Y2 );

	//------ count the number of acquired scrolls ------//

	Nation* nationPtr = nation_array[info.viewing_nation_recno];
	int scrollCount=0;

	int i;
	for( i=0 ; i<MAX_RACE ; i++ )
	{
		if( nationPtr->know_base_array[i] )
			scrollCount++;
	}

	//------- display words -------//

//	if( scrollCount > 1 )
//		font_bld.put( SCROLL_X1+6, SCROLL_Y1+5, "Acquired Scrolls:" );
//	else
//		font_bld.put( SCROLL_X1+6, SCROLL_Y1+5, "Acquired Scroll:" );
	font_bld.put( SCROLL_X1+6, SCROLL_Y1+5, text_reports.str_tech_acq_scroll(scrollCount) );

	//------- display scrolls ----------//

	int x=SCROLL_X1+6, y=SCROLL_Y1+18;

	for( i=0 ; i<MAX_RACE ; i++ )
	{
		if( !nationPtr->know_base_array[i] )
			continue;

		disp_scroll(x, y, i+1 );

		font_bld.put( x+52, y+6, race_res[i+1]->name );

		x+=125;

		if( x+95 > SCROLL_X2 )
		{
			x  = SCROLL_X1+6;
			y += 30;
		}
	}
}
//----------- End of static function disp_owned_scroll -----------//

