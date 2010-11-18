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

//Filename    : OINFOMIL.CPP
//Description : Economy information screen

#include <oinfo.h>
#include <ovga.h>
#include <omodeid.h>
#include <ofont.h>
#include <oconfig.h>
#include <ospy.h>
#include <oimgres.h>
#include <ounitres.h>
#include <oraceres.h>
#include <omonsres.h>
#include <ovbrowif.h> 
#include <obutton.h>
#include <of_camp.h>
#include <onation.h>
#include <ounit.h>
#include <ot_reps.h>
#include <ot_unit.h>

//------------- Define coordinations -----------//

#define TROOP_BROWSE_X1 (ZOOM_X1+6)
#define TROOP_BROWSE_Y1 (ZOOM_Y1+6)
#define TROOP_BROWSE_X2 (ZOOM_X2-6)
#define TROOP_BROWSE_Y2 (TROOP_BROWSE_Y1+240)

#define UNIT_BROWSE_X1 (ZOOM_X1+6)
#define UNIT_BROWSE_Y1 (TROOP_BROWSE_Y2+6)
#define UNIT_BROWSE_X2 (ZOOM_X2-6)
#define UNIT_BROWSE_Y2 (ZOOM_Y2-6)

//----------- Define static variables ----------//

static VBrowseIF browse_troop;
static VBrowseIF browse_unit;

//----------- Define static functions ----------//

static void put_troop_rec(int recNo, int x, int y, int refreshFlag);
static void put_unit_rec(int recNo, int x, int y, int refreshFlag);
static int  troop_filter(int recNo=0);
static int  unit_filter(int recNo=0);
static void	disp_troop_total();
static void disp_unit_total();

//--------- Begin of function Info::disp_military ---------//
//
void Info::disp_military(int refreshFlag)
{
	//------- Display the Troop report -------//

	int x=TROOP_BROWSE_X1+9;
	int y=TROOP_BROWSE_Y1+4;

	vga_back.d3_panel_up(TROOP_BROWSE_X1, TROOP_BROWSE_Y1, TROOP_BROWSE_X2, TROOP_BROWSE_Y1+33 );

	font_bld.put( x	 , y+7 , text_reports.str_mil_commander() ); // "Commander" );
	//SXM
#if(defined(CHINESE))
	font_bld.put( x+210 +12, y+7 , text_unit.str_leadership() ); // "Leadership" );
	font_bld.put( x+287, y+7 , text_unit.str_loyalty() ); // "Loyalty" );
	font_bld.center_put_paragraph( x+358 -4, y+6, x+406, y+24, text_unit.str_hit_points(), 0, 0, 0 );
	font_bld.center_put_paragraph( x+404, y +6, x+487, y+24, text_reports.str_mil_commanded_soldiers(), 0, 0, 0 );
	font_bld.put( x+485, y+7 , text_reports.str_mil_status() ); // "Status" );
#else
	font_bld.put( x+210, y+7 , text_unit.str_leadership() ); // "Leadership" );
	font_bld.put( x+287, y+7 , text_unit.str_loyalty() ); // "Loyalty" );
	//font_bld.put( x+362, y   , "Hit" );
	//font_bld.put( x+352, y+13, "Points" );
	font_bld.center_put_paragraph( x+358, y, x+406, y+24, text_unit.str_hit_points(), 0, 0, 0 );
	// font_bld.put( x+406, y   , "Commanded" );
	// font_bld.put( x+415, y+13, "Soldiers" );
	font_bld.center_put_paragraph( x+404, y, x+487, y+24, text_reports.str_mil_commanded_soldiers(), 0, 0, 0 );
	font_bld.put( x+485, y+7 , text_reports.str_mil_status() ); // "Status" );
#endif
	//SXM

	if( refreshFlag == INFO_REPAINT )
	{
		browse_troop.init( TROOP_BROWSE_X1, TROOP_BROWSE_Y1+35, TROOP_BROWSE_X2, TROOP_BROWSE_Y2-20,
								 0, 22, troop_filter(), put_troop_rec, 1 );

		browse_troop.open(browse_troop_recno);
	}
	else
	{
		browse_troop.paint();
		browse_troop.open(browse_troop_recno, troop_filter());
	}

	disp_troop_total();

	//------- Display the unit report -------//

	x=UNIT_BROWSE_X1+9;
	y=UNIT_BROWSE_Y1+4;

	vga_back.d3_panel_up(UNIT_BROWSE_X1, UNIT_BROWSE_Y1, UNIT_BROWSE_X2, UNIT_BROWSE_Y1+20 );

	font_bld.put( x	 , y, text_reports.str_mil_unit_type() ); // "Unit Type" );
	//SXM
#if(defined(CHINESE))
	font_bld.put( x+300 +12, y, text_reports.str_mil_no_of_units() ); // "No. of Units" );
#else
	font_bld.put( x+300, y, text_reports.str_mil_no_of_units() ); // "No. of Units" );
#endif
	//SXM

	if( refreshFlag == INFO_REPAINT )
	{
		browse_unit.init( UNIT_BROWSE_X1, UNIT_BROWSE_Y1+22, UNIT_BROWSE_X2, UNIT_BROWSE_Y2-20,
								0, 18, unit_filter(), put_unit_rec, 1 );

		browse_unit.open(browse_unit_recno);
	}
	else
	{
		browse_unit.paint();
		browse_unit.open(browse_unit_recno, unit_filter());
	}

	disp_unit_total();
}
//----------- End of function Info::disp_military -----------//


//--------- Begin of function Info::detect_military ---------//
//
void Info::detect_military()
{
	//------- detect the troop report -------//

	if( browse_troop.detect() )
	{
		browse_troop_recno = browse_troop.recno();

		if( browse_troop.double_click )
		{
			Unit* unitPtr = unit_array[ troop_filter(browse_troop_recno) ];
			short xLoc, yLoc;

			if( unitPtr->get_cur_loc(xLoc, yLoc) )
			{
				world.go_loc(xLoc, yLoc, 1);
			}
		}
	}

	//------- detect the unit report -------//

	if( browse_unit.detect() )
	{
		browse_unit_recno = browse_unit.recno();
	}
}
//----------- End of function Info::detect_military -----------//


//-------- Begin of static function disp_troop_total --------//

static void disp_troop_total()
{
	//---- count the total no. of soldiers ----//

	Unit* unitPtr;
	int   totalSoldier=0, totalCommandedSoldier=0;

	for( int i=unit_array.size() ; i>0 ; i-- )
	{
		if( unit_array.is_deleted(i) )
			continue;

		unitPtr = unit_array[i];

		if( unitPtr->nation_recno != info.viewing_nation_recno )
			continue;

		if( unitPtr->rank_id == RANK_SOLDIER )
			totalSoldier++;
		else
			totalCommandedSoldier += unitPtr->commanded_soldier_count();
	}

	Nation* viewingNation = nation_array[info.viewing_nation_recno];

	for( i=viewingNation->ai_camp_count-1 ; i>=0 ; i-- )
	{
		//totalSoldier += firm_array[ viewingNation->ai_camp_array[i] ]->worker_count;
		FirmCamp *firmCamp;
		if( (firmCamp = firm_array[viewingNation->ai_camp_array[i]]->cast_to_FirmCamp()) )
		{
			totalSoldier += firmCamp->soldier_count;
		}
	}

	//--------- paint the area ----------//

	int x=TROOP_BROWSE_X1+9;
	int y=TROOP_BROWSE_Y2-16;

	vga_back.d3_panel_up(TROOP_BROWSE_X1, TROOP_BROWSE_Y2-18, TROOP_BROWSE_X2, TROOP_BROWSE_Y2 );

	//------ display commander count ------//

//	String str;

//	if( browse_troop.total_rec() > 1 )
//		str = "Total Commanders";
//	else
//		str = "Total Commander";
//	str  = translate.process(str);
//	str += ": ";
//	str += browse_troop.total_rec();
	font_bld.put( x, y, text_reports.str_total_commander(browse_troop.total_rec()) );

	//------ display soldiers under command count ------//

#if(defined(FRENCH))
	#define COMMANDED_SOLDIER_SHIFT -10
#else
	#define COMMANDED_SOLDIER_SHIFT 0
#endif
//	if( totalCommandedSoldier > 1 )
//		str = "Total Soldiers Under Command";
//	else
//		str = "Total Soldier Under Command";
//	str  = translate.process(str);
//	str += ": ";
//	str += totalCommandedSoldier;
	font_bld.put( x+160+COMMANDED_SOLDIER_SHIFT, y, text_reports.str_total_soldier_under(totalCommandedSoldier) );
#undef COMMANDED_SOLDIER_SHIFT


	//------ display soldiers count ------//

//	if( totalSoldier > 1 )
//		str = "Total Soldiers";
//	else
//		str = "Total Soldier";
//	str  = translate.process(str);
//	str += ": ";
//	str += totalSoldier;

#if(defined(FRENCH))
	#define TOTAL_SOLDIER_SHIFT 10
#else
	#define TOTAL_SOLDIER_SHIFT 0
#endif
	font_bld.put( x+410+TOTAL_SOLDIER_SHIFT, y, text_reports.str_total_soldier(totalSoldier) );
#undef TOTAL_SOLDIER_SHIFT
}
//----------- End of static function disp_troop_total -----------//


//-------- Begin of static function disp_unit_total --------//

static void disp_unit_total()
{
	int totalUnitCount=0, totalUnitCost=0;

	UnitInfo* unitInfo;

	for( int unitId=1 ; unitId<=MAX_UNIT_TYPE ; unitId++ )
	{
		unitInfo = unit_res[unitId];

		int unitCount    = unitInfo->nation_unit_count_array[info.viewing_nation_recno-1];
		int generalCount = unitInfo->nation_general_count_array[info.viewing_nation_recno-1];

		totalUnitCount += unitCount;

		if( unitInfo->race_id )
		{
			totalUnitCost += generalCount * GENERAL_YEAR_SALARY;
			totalUnitCost += (unitCount-generalCount) * SOLDIER_YEAR_SALARY;
		}
		else
			totalUnitCost += unitCount * unitInfo->year_cost;
	}


	//--------- paint the area ----------//

	int x=UNIT_BROWSE_X1+9;
	int y=UNIT_BROWSE_Y2-16;

	vga_back.d3_panel_up(UNIT_BROWSE_X1, UNIT_BROWSE_Y2-18, UNIT_BROWSE_X2, UNIT_BROWSE_Y2 );

	// String str;
	// str  = "Total Units";
	// str  = translate.process(str);
	// str += ": ";
	//str += totalUnitCount;
	font_bld.put( x, y, text_reports.str_total_unit(totalUnitCount) );
}
//----------- End of static function disp_unit_total -----------//


//-------- Begin of static function troop_filter --------//
//
// This function has dual purpose :
//
// 1. when <int> recNo is not given :
//    - return the total no. of firms of this nation
//
// 2. when <int> recNo is given :
//    - return the firm recno in firm_array of the given recno.
//
static int troop_filter(int recNo)
{
	int   i, unitCount=0, totalUnit=unit_array.size();
	Unit* unitPtr;

	for( i=1 ; i<=totalUnit ; i++ )
	{
		if( unit_array.is_deleted(i) )
			continue;

		unitPtr = unit_array[i];

		if( unitPtr->nation_recno == info.viewing_nation_recno &&
			 (unitPtr->rank_id==RANK_GENERAL || unitPtr->rank_id==RANK_KING) )
		{
			unitCount++;
		}

		if( recNo && unitCount==recNo )
			return i;
	}

	err_when( recNo );   // the recNo is not found, it is out of range

	return unitCount;
}
//----------- End of static function troop_filter -----------//


//-------- Begin of static function put_troop_rec --------//
//
static void put_troop_rec(int recNo, int x, int y, int refreshFlag)
{
	int   unitRecno = troop_filter(recNo);
	Unit* unitPtr   = unit_array[unitRecno];

	//---------- display bitmap ----------//

	x+=3;
	y+=3;

	//-------- display name & team size -------//

	y+=2;

	font_bld.put( x    , y, unitPtr->unit_name(1, 1), 0, x+178 );

	char* raceName;

	if( unitPtr->race_id > 0 )
		raceName = race_res[unitPtr->race_id]->name;
	else
		raceName = monster_res[unitPtr->monster_id()]->name;

	font_bld.put( x+180, y, raceName, 0, x+248 );

	font_bld.put( x+250, y, unitPtr->skill_level() );

	if( unitPtr->rank_id != RANK_KING )
		font_bld.put( x+295, y, unitPtr->loyalty );

	//--------- display hit points -----------//

	String str;

	str  = (int) unitPtr->hit_points;
	str += "/";
	str += unitPtr->max_hit_points();

	font_bld.put( x+352, y, str );

	//---- display the no. of soldiers led by this general ---//

	font_bld.put( x+445, y, unitPtr->commanded_soldier_count() );

	//---- display the status of the general ----//

//	char* statusStr;
//	if( unitPtr->unit_mode == UNIT_MODE_OVERSEE )
//	{
//		statusStr = "In Fort";
//	}
//	else if( unitPtr->unit_mode == UNIT_MODE_ON_SHIP )
//	{
//		statusStr = "On Ship";
//	}
//	else
//	{
//		statusStr = "Mobile";
//	}

	font_bld.put( x+478, y, text_reports.str_mil_unit_status(unitPtr->unit_mode) );
}
//----------- End of static function put_troop_rec -----------//


//-------- Begin of static function unit_filter --------//
//
// This function has dual purpose :
//
// 1. when <int> recNo is not given :
//    - return the total no. of firms of this nation
//
// 2. when <int> recNo is given :
//    - return the firm recno in firm_array of the given recno.
//
static int unit_filter(int recNo)
{
	int 		 unitTypeCount=0;
	UnitInfo* unitInfo;

	//------- count human units -------//

	Nation* nationPtr = nation_array[info.viewing_nation_recno];

	unitTypeCount++;

	if( recNo && unitTypeCount==recNo )
		return -1;

	//------- count non-human unit types -------//

	for( int unitId=1 ; unitId<=MAX_UNIT_TYPE ; unitId++ )
	{
		unitInfo = unit_res[unitId];

		//---- if this is not a human unit -----//

		if( !unitInfo->race_id &&
			 unitInfo->nation_unit_count_array[info.viewing_nation_recno-1] > 0 )
		{
			unitTypeCount++;
		}

		if( recNo && unitTypeCount==recNo )
			return unitId;
	}

	err_when( recNo );   // the recNo is not found, it is out of range

	return unitTypeCount;
}
//----------- End of static function unit_filter -----------//


//-------- Begin of static function put_unit_rec --------//
//
static void put_unit_rec(int recNo, int x, int y, int refreshFlag)
{
	int   	 unitId = unit_filter(recNo);
	int   	 unitCount;
	char* 	 str;
	UnitInfo* unitInfo;
	Nation* nationPtr = nation_array[info.viewing_nation_recno];
	int rc = unit_filter(recNo);

	switch( rc )
	{
		//------- count human units -------//

		case -1:
			str		  = text_unit.str_title( nationPtr->race_id, RANK_GENERAL ); // "General";
			unitCount  = nationPtr->total_general_count;
			break;

		default:
			unitInfo   = unit_res[rc];
			str 		  = unitInfo->name;
			unitCount  = unitInfo->nation_unit_count_array[ info.viewing_nation_recno-1 ];
	}

	//---------- display info --------//

	x+=3;
	y+=2;

	font_bld.put( x   , y, str );
	font_bld.put( x+320, y, m.format(unitCount,1) );
}
//----------- End of static function put_unit_rec -----------//


