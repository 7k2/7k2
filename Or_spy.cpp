//Filename    : OR_SPY.CPP
//Description : Spy Report

#include <OINFO.H>
#include <OVGA.H>
#include <OMODEID.H>
#include <OFONT.H>
#include <OFIRMRES.H>
#include <ONATION.H>
#include <ORACERES.H>
#include <OIMGRES.H>
#include <OVBROWIF.H>
#include <OSPY.H>
// #include <OWORLDMT.H>
#include <OT_REPS.H>
#include <OT_UNIT.H>

//------------- Define coordinations -----------//

//enum { SPY_BROWSE_X1 = ZOOM_X1+6,
//		 SPY_BROWSE_Y1 = ZOOM_Y1+6,
//		 SPY_BROWSE_X2 = ZOOM_X2-6,
//		 SPY_BROWSE_Y2 = ZOOM_Y2-6,
//	  };
#define SPY_BROWSE_X1 (ZOOM_X1+6)
#define SPY_BROWSE_Y1 (ZOOM_Y1+6)
#define SPY_BROWSE_X2 (ZOOM_X2-6)
#define SPY_BROWSE_Y2 (ZOOM_Y2-6)

//----------- Define static variables ----------//

static VBrowseIF browse_spy;

//----------- Define static functions ----------//

static void put_spy_rec(int recNo, int x, int y, int refreshFlag);
static int  spy_filter(int recNo=0);
static void	disp_total();

//--------- Begin of function Info::disp_spy ---------//
//
void Info::disp_spy(int refreshFlag)
{
	//------- Display the spy report -------//

	int x=SPY_BROWSE_X1+9;
	int y=SPY_BROWSE_Y1+4;

#if(defined(FRENCH))
	#define SPY_BROWSE_HEIGHT 36
#else
	#define SPY_BROWSE_HEIGHT 21
#endif

	vga_back.d3_panel_up(SPY_BROWSE_X1, SPY_BROWSE_Y1, SPY_BROWSE_X2, SPY_BROWSE_Y1+SPY_BROWSE_HEIGHT-1 );

	font_bld.put( x	 , y, text_reports.str_spy_name() ); //"Spy Name" );
	font_bld.put( x+150, y, text_reports.str_nationality() ); // "Nationality" );
#if(defined(CHINESE))
	font_bld.put( x+250 +2, y, text_reports.str_spy_cloak() ); // "Cloak" );
#else
	font_bld.put( x+250, y, text_reports.str_spy_cloak() ); // "Cloak" );
#endif
#if(defined(CHINESE))
	font_bld.put( x+305 +4, y, text_reports.str_spy_location() ); //"Location" );
	font_bld.put( x+380, y, text_reports.str_skill() ); // "Skill" );
	font_bld.put( x+424 -6, y, text_unit.str_loyalty() ); //"Loyalty" );
	font_bld.put( x+490 +6, y, text_reports.str_spy_action() ); //"Action" );
#elif(defined(FRENCH))
	font_bld.put( x+305, y+14, text_reports.str_spy_location() ); //"Location" );
	font_bld.put( x+380, y, text_reports.str_skill() ); // "Skill" );
	font_bld.put( x+424, y+14, text_unit.str_loyalty() ); //"Loyalty" );
	font_bld.put( x+490, y, text_reports.str_spy_action() ); //"Action" );
#elif(defined(GERMAN))
	font_bld.put( x+315, y, text_reports.str_spy_location() ); //"Location" );
	font_bld.put( x+365, y, text_reports.str_skill() ); // "Skill" );
	font_bld.put( x+414, y, text_unit.str_loyalty() ); //"Loyalty" );
	font_bld.put( x+490, y, text_reports.str_spy_action() ); //"Action" );
#else
	// US
	font_bld.put( x+305, y, text_reports.str_spy_location() ); //"Location" );
	font_bld.put( x+380, y, text_reports.str_skill() ); // "Skill" );
	font_bld.put( x+424, y, text_unit.str_loyalty() ); //"Loyalty" );
	font_bld.put( x+490, y, text_reports.str_spy_action() ); //"Action" );
#endif

	if( refreshFlag == INFO_REPAINT )
	{
		browse_spy.init( SPY_BROWSE_X1, SPY_BROWSE_Y1+SPY_BROWSE_HEIGHT+1, SPY_BROWSE_X2, SPY_BROWSE_Y2-20,
								 0, 21, spy_filter(), put_spy_rec, 1 );

		browse_spy.open(browse_spy_recno);
	}
	else
	{
		browse_spy.paint();
		browse_spy.open(browse_spy_recno, spy_filter());
	}

	//--------- Display total ------------//

	disp_total();
}
//----------- End of function Info::disp_spy -----------//


//--------- Begin of function Info::detect_spy ---------//
//
void Info::detect_spy()
{
	//------- detect the spy browser -------//

	if( browse_spy.detect() )
	{
		browse_spy_recno = browse_spy.recno();

		if( browse_spy.double_click )
		{
			Spy* spyPtr = spy_array[ spy_filter(browse_spy_recno) ];
			int  xLoc, yLoc;

			if( spyPtr->get_loc(xLoc, yLoc) )
				world.go_loc( xLoc, yLoc, 1 );
		}
	}
}
//----------- End of function Info::detect_spy -----------//


//-------- Begin of static function disp_total --------//

static void disp_total()
{
	int x = SPY_BROWSE_X1+9;
	int y = SPY_BROWSE_Y2-16;

	vga_back.d3_panel_up(SPY_BROWSE_X1, SPY_BROWSE_Y2-18, SPY_BROWSE_X2, SPY_BROWSE_Y2 );

//	String str;
//	if( browse_spy.total_rec() > 1 )
//		str = "Total Spies";
//	else
//		str = "Total Spy";
//	str  = translate.process(str);
//	str += ": ";
//	str += browse_spy.total_rec();
	font_bld.put( x, y, text_reports.str_total_spy(browse_spy.total_rec()) );
}
//----------- End of static function disp_total -----------//


//-------- Begin of static function spy_filter --------//
//
// This function has dual purpose :
//
// 1. when <int> recNo is not given :
//    - return the total no. of firms of this nation
//
// 2. when <int> recNo is given :
//    - return the firm recno in firm_array of the given recno.
//
static int spy_filter(int recNo)
{
	int  totalSpy = spy_array.size();
	int  spyCount=0;
	Spy* spyPtr;

	for( int spyRecno=1 ; spyRecno<=totalSpy ; spyRecno++ )
	{
		if( spy_array.is_deleted(spyRecno) )
			continue;

		spyPtr = spy_array[spyRecno];

		if( spyPtr->true_nation_recno==info.viewing_nation_recno )
			spyCount++;

		if( recNo && spyCount==recNo )
			return spyRecno;
	}

	err_when( recNo );   // the recNo is not found, it is out of range

	return spyCount;
}
//----------- End of static function spy_filter -----------//


//-------- Begin of static function put_spy_rec --------//
//
static void put_spy_rec(int recNo, int x, int y, int refreshFlag)
{
	int  spyRecno = spy_filter(recNo);
	Spy* spyPtr   = spy_array[spyRecno];

	x+=3;
	y+=5;

	//----------- display name -----------//

	int	 raceId = spyPtr->race_id;
	int 	 cloakedRankId  = spyPtr->cloaked_rank_id();
	int 	 cloakedSkillLevel = spyPtr->cloaked_skill_level();


//	String str;
//	if( spyPtr->is_civilian && raceId > 0 )
//	{
//		str = "Civilian ";
//	}
//	else
//	{
//		switch( cloakedRankId )
//		{
//			case RANK_KING:
//				if( raceId > 0 )
//					str = "King ";
//				else
//					str = "All High ";
//				break;
//
//			case RANK_GENERAL:
//				if( raceId > 0 )
//					str = "General ";
//				else
//					str = "Ordo ";
//				break;
//
//			case RANK_SOLDIER:
//				if( raceId > 0 )
//					str = "Soldier ";
//				else
//					str  = "Croul ";
//		}
//	}
//	str += race_res[raceId]->get_name(spyPtr->name_id);

	err_when( spyPtr->race_id <= 0 );	// only human spy so far

	char *titledName;
	if( spyPtr->is_civilian )
		titledName = text_unit.str_civilian_titled( 
			race_res[spyPtr->race_id]->get_name(spyPtr->name_id) );
	else
		titledName = text_unit.str_unit_titled(spyPtr->race_id, 
			race_res[spyPtr->race_id]->get_name(spyPtr->name_id), 
			spyPtr->cloaked_rank_id() );

	font_bld.put( x, y, titledName, 0, x+145 );

	//------- display nationality --------//

	font_bld.put( x+150, y, race_res[raceId]->name, 0, x+250 );

	//------- display cloaked nation color ------//

	int tx = x+260;

	if( spyPtr->cloaked_nation_recno==0 )		// independent nation
	{
		nation_array.disp_nation_color(tx, y+2, V_WHITE);
	}
	else
	{
		nation_array[spyPtr->cloaked_nation_recno]->disp_nation_color(tx, y+2);
	}

	//---------- display other info ----------//

	char* placeStr;
	switch( spyPtr->spy_place )
	{
		case SPY_FIRM:
			// ###### patch begin Gilbert 27/9 ######//
			// placeStr = firm_res[firm_array[spyPtr->spy_place_para]->firm_id]->name;
			placeStr = firm_res.get_build(firm_array[spyPtr->spy_place_para]->firm_build_id)->name;
			// ###### patch end Gilbert 27/9 ######//
			break;

		case SPY_TOWN:
			placeStr = town_array[spyPtr->spy_place_para]->town_name();
			break;

		case SPY_MOBILE:
		{
			Unit* unitPtr = unit_array[spyPtr->spy_place_para];

			switch( unitPtr->unit_mode )
			{
				case UNIT_MODE_CONSTRUCT_FIRM:
					placeStr = firm_res[firm_array[unitPtr->unit_mode_para]->firm_id]->name;
					break;

				case UNIT_MODE_CONSTRUCT_TOWN:
					placeStr = town_array[unitPtr->unit_mode_para]->town_name();
					break;
//				case UNIT_MODE_ON_SHIP:
//					placeStr = "On Ship";
//					break;
//				default:
//					placeStr = "Mobile";
				default:
					placeStr = text_reports.str_mil_unit_status( unitPtr->unit_mode );
			}
			break;
		}

		default:
			placeStr = "";
	}

	font_bld.put( x+305, y, placeStr );

	font_bld.put( x+390, y, spyPtr->spy_skill );
	font_bld.put( x+420, y, spyPtr->spy_loyalty );
	font_bld.right_put( x+525, y, spyPtr->action_str() );
}
//----------- End of static function put_spy_rec -----------//


