//Filename    : OR_TOWN.CPP
//Description : Town Report

#include <OINFO.H>
#include <OVGA.H>
#include <OMODEID.H>
#include <OFONT.H>
#include <OIMGRES.H>
#include <OVBROWIF.H>
#include <OBUTTON.H>
#include <ORACERES.H>
#include <OFIRM.H> 
#include <OFIRM.H>
#include <OWORLD.H>
#include <ONATION.H>
#include <OUNIT.H>
// #include <OWORLDMT.H>
#include <OT_REPS.H>
#include <OT_UNIT.H>


//------------- Define coordinations -----------//

//enum { TOWN_BROWSE_X1 = ZOOM_X1+6,
//		 TOWN_BROWSE_Y1 = ZOOM_Y1+6,
//		 TOWN_BROWSE_X2 = ZOOM_X2-6,
//		 TOWN_BROWSE_Y2 = TOWN_BROWSE_Y1+280
//	  };
#define TOWN_BROWSE_X1 (ZOOM_X1+6)
#define TOWN_BROWSE_Y1 (ZOOM_Y1+6)
#define TOWN_BROWSE_X2 (ZOOM_X2-6)
#define TOWN_BROWSE_Y2 (TOWN_BROWSE_Y1+280)

//enum { POP_TOTAL_X1 = ZOOM_X1+6,
//		 POP_TOTAL_Y1 = TOWN_BROWSE_Y2+2,
//		 POP_TOTAL_X2 = ZOOM_X2-6,
//		 POP_TOTAL_Y2 = POP_TOTAL_Y1+18,
//	  };
#define POP_TOTAL_X1 (ZOOM_X1+6)
#define POP_TOTAL_Y1 (TOWN_BROWSE_Y2+2)
#define POP_TOTAL_X2 (ZOOM_X2-6)
#define POP_TOTAL_Y2 (POP_TOTAL_Y1+18)

//enum { FIRM_BROWSE_X1 = ZOOM_X1+6,
//		 FIRM_BROWSE_Y1 = POP_TOTAL_Y2+6,
//		 FIRM_BROWSE_X2 = ZOOM_X2-6,
//		 FIRM_BROWSE_Y2 = ZOOM_Y2-6,
//	  };
#define FIRM_BROWSE_X1 (ZOOM_X1+6)
#define FIRM_BROWSE_Y1 (POP_TOTAL_Y2+6)
#define FIRM_BROWSE_X2 (ZOOM_X2-6)
#define FIRM_BROWSE_Y2 (ZOOM_Y2-6)

//----------- Define static variables ----------//

static VBrowseIF browse_town, browse_firm;
static int		  total_population, total_peasant;
static int  	  firm_income_array[MAX_FIRM_TYPE];
static int  	  total_firm_cost, total_firm_income, total_firm_count;
static float	  total_expense;

//----------- Define static functions ----------//

static void put_town_rec(int recNo, int x, int y, int refreshFlag);
static void put_firm_rec(int recNo, int x, int y, int refreshFlag);
static int  town_filter(int recNo=0);
static int  firm_filter(int recNo=0);
static void	disp_total();
static void calc_firm_total();

//--------- Begin of function Info::disp_town ---------//
//
void Info::disp_town(int refreshFlag)
{
	int x=TOWN_BROWSE_X1+9;
	int y=TOWN_BROWSE_Y1+4;

	vga_back.d3_panel_up(TOWN_BROWSE_X1, TOWN_BROWSE_Y1, TOWN_BROWSE_X2, TOWN_BROWSE_Y1+20 );

	//SXM
#if(defined(CHINESE))
	font_bld.put( x	 , y, text_reports.str_town() ); // "Town" );
	font_bld.put( x+150 +18, y, text_reports.str_citizens() ); // "Citizens" );
	font_bld.put( x+225 +10, y, text_reports.str_peasants() ); // "Peasants" );
	font_bld.put( x+300 +8, y, text_unit.str_loyalty() ); // "Loyalty" );
	font_bld.put( x+400 +6, y, text_reports.str_nationality() ); // "Nationality" );
#else
	font_bld.put( x	 , y, text_reports.str_town() ); // "Town" );
	font_bld.put( x+150, y, text_reports.str_citizens() ); // "Citizens" );
	font_bld.put( x+225, y, text_reports.str_peasants() ); // "Peasants" );
	font_bld.put( x+300, y, text_unit.str_loyalty() ); // "Loyalty" );
	font_bld.put( x+400, y, text_reports.str_nationality() ); // "Nationality" );
#endif
	//SXM
	if( refreshFlag == INFO_REPAINT )
	{
		browse_town.init( TOWN_BROWSE_X1, TOWN_BROWSE_Y1+22, TOWN_BROWSE_X2, TOWN_BROWSE_Y2-20,
								0, 21, town_filter(), put_town_rec, 1 );

		browse_town.open(browse_town_recno);
	}
	else
	{
		browse_town.paint();
		browse_town.open(browse_town_recno, town_filter());
	}

	//------- Display the firm report -------//

	calc_firm_total();

	x=FIRM_BROWSE_X1+9;
	y=FIRM_BROWSE_Y1+4;

#if(defined(FRENCH))
	// two-line label
	#define FIRM_LABEL_HEIGHT 36
	vga_back.d3_panel_up(FIRM_BROWSE_X1, FIRM_BROWSE_Y1, FIRM_BROWSE_X2, FIRM_BROWSE_Y1+FIRM_LABEL_HEIGHT-1 );
	font_bld.put( x	  , y+8, text_reports.str_structure() ); // "Structure" );
	font_bld.center_put_paragraph( x+130, y, x+215, y+FIRM_LABEL_HEIGHT-8, text_reports.str_unit_cost(), DEFAULT_LINE_SPACE, 0, 0 ); // "Unit Cost" );
	font_bld.center_put_paragraph( x+217, y, x+328, y+FIRM_LABEL_HEIGHT-8, text_reports.str_no_of_structures(), DEFAULT_LINE_SPACE, 0, 0  ); // "No. of Structures" );
	font_bld.center_put_paragraph( x+340, y, x+438, y+FIRM_LABEL_HEIGHT-8, text_reports.str_yearly_expense(), DEFAULT_LINE_SPACE, 0, 0  ); // "Yearly Expense" );
	font_bld.center_put_paragraph( x+440, y, x+548, y+FIRM_LABEL_HEIGHT-8, text_reports.str_yearly_income(), DEFAULT_LINE_SPACE, 0, 0  ); // "Yearly Income" );
#else	
	// one-line label
	#define FIRM_LABEL_HEIGHT 21
	vga_back.d3_panel_up(FIRM_BROWSE_X1, FIRM_BROWSE_Y1, FIRM_BROWSE_X2, FIRM_BROWSE_Y1+FIRM_LABEL_HEIGHT-1 );

	//SXM
	#if(defined(CHINESE))
	font_bld.put( x	 , y, text_reports.str_structure() ); // "Structure" );
	font_bld.put( x+140, y, text_reports.str_unit_cost() ); // "Unit Cost" );
	font_bld.put( x+217 +16, y, text_reports.str_no_of_structures() ); // "No. of Structures" );
	font_bld.put( x+340 +28, y, text_reports.str_yearly_expense() ); // "Yearly Expense" );
	font_bld.put( x+450 +17, y, text_reports.str_yearly_income() ); // "Yearly Income" );
	#elif(defined(ITALIAN))
	font_bld.put( x	 , y, text_reports.str_structure() ); // "Structure" );
	font_bld.put( x+110, y, text_reports.str_unit_cost() ); // "Unit Cost" );
	font_bld.put( x+217, y, text_reports.str_no_of_structures() ); // "No. of Structures" );
	font_bld.put( x+340, y, text_reports.str_yearly_expense() ); // "Yearly Expense" );
	font_bld.put( x+450, y, text_reports.str_yearly_income() ); // "Yearly Income" );
	#else // GERMAN and US
	font_bld.put( x	 , y, text_reports.str_structure() ); // "Structure" );
	font_bld.put( x+140, y, text_reports.str_unit_cost() ); // "Unit Cost" );
	font_bld.put( x+217, y, text_reports.str_no_of_structures() ); // "No. of Structures" );
	font_bld.put( x+340, y, text_reports.str_yearly_expense() ); // "Yearly Expense" );
	font_bld.put( x+450, y, text_reports.str_yearly_income() ); // "Yearly Income" );
	#endif
	//SXM
#endif

	if( refreshFlag == INFO_REPAINT )
	{
		// ###### begin Gilbert 19/10 ######//
		browse_firm.init( FIRM_BROWSE_X1, FIRM_BROWSE_Y1+FIRM_LABEL_HEIGHT+1, FIRM_BROWSE_X2, FIRM_BROWSE_Y2-20,
								0, 18, firm_filter(), put_firm_rec, 1 );
		// ###### end Gilbert 19/10 ######//

		browse_firm.open(browse_firm_recno);
	}
	else
	{
		browse_firm.paint();
		browse_firm.open(browse_firm_recno, firm_filter());
	}
#undef FIRM_LABEL_HEIGHT

	//--------- Display total ------------//

	disp_total();
}
//----------- End of function Info::disp_town -----------//


//--------- Begin of function Info::detect_town ---------//
//
void Info::detect_town()
{
   //------- detect the town browser -------//

	if( browse_town.detect() )
	{
		browse_town_recno = browse_town.recno();

		if( browse_town.double_click )
		{
			Town* townPtr = town_array[ town_filter(browse_town_recno) ];

			world.go_loc(townPtr->center_x, townPtr->center_y, 1);
		}
	}

	//------- detect the firm browser -------//

	if( browse_firm.detect() )
		browse_firm_recno = browse_firm.recno();
}
//----------- End of function Info::detect_town -----------//


//-------- Begin of static function disp_total --------//

static void disp_total()
{
	//------- calculate total --------//

	total_population = 0;
	total_peasant	  = 0;

	Town* townPtr;

	for( int i=town_array.size() ; i>0 ; i-- )
	{
		if( town_array.is_deleted(i) )
			continue;

		townPtr = town_array[i];

		if( townPtr->nation_recno==info.viewing_nation_recno )
		{
			total_population += townPtr->population;
			total_peasant	  += townPtr->jobless_population;
		}
	}

	//-------- display town total --------//

	int x=TOWN_BROWSE_X1+9;
	int y=TOWN_BROWSE_Y2-16;

	vga_back.d3_panel_up(TOWN_BROWSE_X1, TOWN_BROWSE_Y2-18, TOWN_BROWSE_X2, TOWN_BROWSE_Y2 );

//	String str;

//	if( browse_town.total_rec() > 1 )
//		str = "Total Towns";
//	else
//		str = "Total Town";
//	str  = translate.process(str);
//	str += ": ";
//	str += browse_town.total_rec();
	font_bld.put( x, y, text_reports.str_total_towns(browse_town.total_rec()) ); // str );

   //-------------------------------//

//	str  = "Total Citizens";
//	str  = translate.process(str);
//	str += ": ";
//	str += total_population;
	font_bld.put( x+180, y, text_reports.str_total_citizens(total_population) ); // str );

	//-------------------------------//

//	if( total_peasant > 1 )
//		str = "Total Peasants";
//	else
//		str = "Total Peasant";
//	str  = translate.process(str);
//	str += ": ";
//	str += total_peasant;
	font_bld.put( x+360, y, text_reports.str_total_peasant(total_peasant) ); // str );

	//------- display other totals --------//

	Nation* viewNation = nation_array[info.viewing_nation_recno];

	x=POP_TOTAL_X1+9;
	y=POP_TOTAL_Y1+2;

	vga_back.d3_panel_up(POP_TOTAL_X1, POP_TOTAL_Y1, POP_TOTAL_X2, POP_TOTAL_Y2 );

//	str  = "Total Other Units";
//	str  = translate.process(str);
//	str += ": ";
//	str += viewNation->total_human_count + viewNation->total_monster_count;
	font_bld.put( x, y, text_reports.str_total_other_units(viewNation->total_human_count + viewNation->total_monster_count) ); // str );

//	str  = "Total Population";
//	str  = translate.process(str);
//	str += ": ";
//	str += viewNation->total_population + viewNation->total_human_count
//			 + viewNation->total_monster_count;
	font_bld.put( x+360, y, text_reports.str_total_population(
		total_population + viewNation->total_human_count + viewNation->total_monster_count) ); // str );
	// use total_population instead of viewNation->total_population because
	// total_population is updated frequently in report mode

	//-------- display firm total ---------//

	x=FIRM_BROWSE_X1+9;
	y=FIRM_BROWSE_Y2-16;

	vga_back.d3_panel_up(FIRM_BROWSE_X1, FIRM_BROWSE_Y2-18, FIRM_BROWSE_X2, FIRM_BROWSE_Y2 );

	font_bld.put( x	 , y, text_reports.str_total() ); // "Total" );
	font_bld.put( x+265, y, total_firm_count );
	font_bld.put( x+370, y, m.format(total_firm_cost,2) );
	font_bld.put( x+470, y, m.format(total_firm_income,2) );
}
//----------- End of static function disp_total -----------//


//-------- Begin of static function calc_firm_total --------//

static void calc_firm_total()
{
	//-------- calculate firm incomes --------//

	total_firm_income = 0;

	memset( firm_income_array, 0, sizeof(firm_income_array) );

	int   thisIncome;
	Firm* firmPtr;

	for( int i=firm_array.size() ; i>0 ; i-- )
	{
		if( firm_array.is_deleted(i) )
			continue;

		firmPtr = firm_array[i];

		if( firmPtr->nation_recno == info.viewing_nation_recno )
		{
			thisIncome = (int) firmPtr->income_365days();

			if( thisIncome > 0 )
			{
				firm_income_array[firmPtr->firm_id-1] += thisIncome;
				total_firm_income += thisIncome;
			}
		}
	}

	//------ calculate total firm cost --------//

	total_firm_count = 0;
	total_firm_cost  = 0;

	FirmInfo* firmInfo;

	for( i=1 ; i<=MAX_FIRM_TYPE ; i++ )
	{
		firmInfo = firm_res[i];

		total_firm_cost += firmInfo->year_cost *
								 firmInfo->nation_firm_count_array[info.viewing_nation_recno-1];

		total_firm_count += firmInfo->nation_firm_count_array[info.viewing_nation_recno-1];
	}
}
//----------- End of static function calc_firm_total -----------//


//-------- Begin of static function town_filter --------//
//
// This function has dual purpose :
//
// 1. when <int> recNo is not given :
//    - return the total no. of firms of this nation
//
// 2. when <int> recNo is given :
//    - return the firm recno in firm_array of the given recno.
//
static int town_filter(int recNo)
{
	int   totalTown = town_array.size();
	int   townCount=0;
	Town* townPtr;

	for( int townRecno=1 ; townRecno<=totalTown ; townRecno++ )
	{
		if( town_array.is_deleted(townRecno) )
			continue;

		townPtr = town_array[townRecno];

		if( townPtr->nation_recno==info.viewing_nation_recno )
			townCount++;

		if( recNo && townCount==recNo )
			return townRecno;
	}

	err_when( recNo );   // the recNo is not found, it is out of range

	return townCount;
}
//----------- End of static function town_filter -----------//


//-------- Begin of static function put_town_rec --------//
//
static void put_town_rec(int recNo, int x, int y, int refreshFlag)
{
	int   townRecno = town_filter(recNo);
	Town* townPtr   = town_array[townRecno];

	//---------- display info ----------//

	x+=3;
	y+=3;

	font_bld.put( x    , y, townPtr->town_name() );
	font_bld.put( x+175, y, townPtr->population );
	font_bld.put( x+241, y, townPtr->jobless_population );

	String str;
	str  = (int) townPtr->loyalty;
	str += " -> ";
	str += (int) townPtr->target_loyalty;

	font_bld.put( x+300, y, str );

	font_bld.put( x+400, y, race_res[townPtr->race_id]->name );
}
//----------- End of static function put_town_rec -----------//


//-------- Begin of static function firm_filter --------//
//
// This function has dual purpose :
//
// 1. when <int> recNo is not given :
//    - return the total no. of firms of this nation
//
// 2. when <int> recNo is given :
//    - return the firm recno in firm_array of the given recno.
//
static int firm_filter(int recNo)
{
	int 		 firmTypeCount=0;
	FirmInfo* firmInfo;

	for( int firmId=1 ; firmId<=MAX_FIRM_TYPE ; firmId++ )
	{
		firmInfo = firm_res[firmId];

		if( firmInfo->nation_firm_count_array[info.viewing_nation_recno-1] )
			firmTypeCount++;

		if( recNo && firmTypeCount==recNo )
			return firmId;
	}

	err_when( recNo );   // the recNo is not found, it is out of range

	return firmTypeCount;
}
//----------- End of static function firm_filter -----------//


//----------- Begin of static function put_firm_rec -----------//
//
static void put_firm_rec(int recNo, int x, int y, int refreshFlag)
{
	int   	 firmId = firm_filter(recNo);
	FirmInfo* firmInfo = firm_res[firmId];

	x+=3;
	// ##### begin Gilbert 19/10 ######//
	y+=2;
	// ##### end Gilbert 19/10 ######//

	int firmCount = firmInfo->nation_firm_count_array[info.viewing_nation_recno-1];

	font_bld.put( x    , y, firmInfo->name );
	font_bld.put( x+155, y, m.format(firmInfo->year_cost,2) );
	font_bld.put( x+265, y, firmCount );
	font_bld.put( x+370, y, m.format(firmInfo->year_cost*firmCount,2) );
	font_bld.put( x+470, y, m.format(firm_income_array[firmId-1], 2) );
}
//----------- End of static function put_firm_rec -----------//

