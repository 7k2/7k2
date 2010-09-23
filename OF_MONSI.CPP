//Filename    : OF_MONSI.CPP
//Description : Firm Monster

#include <OF_MONS.H>
#include <OMONSRES.H>
#include <OU_MONS.H>
#include <OINFO.H>
#include <OVGA.H>
#include <OMODEID.H>
#include <ODATE.H>
#include <OSTR.H>
#include <OSYS.H>
#include <OCONFIG.H>
#include <OSITE.H>
#include <OFONT.H>
#include <OBUTTON.H>
#include <OTOWN.H>
#include <ONATION.H>



//--------- Begin of function FirmMonster::put_info ---------//
//
void FirmMonster::put_info(int refreshFlag)
{
/*
	disp_basic_info(INFO_Y1, refreshFlag);

	if( !config.show_ai_info && nation_recno!=nation_array.player_recno )
		return;

	disp_monster_info(INFO_Y1+54, refreshFlag);
*/
}
//----------- End of function FirmMonster::put_info -----------//


//--------- Begin of function FirmMonster::detect_info ---------//
//
void FirmMonster::detect_info()
{
/*
	if( detect_basic_info() )
		return;

	if( !config.show_ai_info && nation_recno!=nation_array.player_recno )
		return;
*/
}
//----------- End of function FirmMonster::detect_info -----------//


/*
//--------- Begin of function FirmMonster::disp_monster_info ---------//
//
void FirmMonster::disp_monster_info(int dispY1, int refreshFlag)
{
	return;

	//---------------- paint the panel --------------//

	if( refreshFlag == INFO_REPAINT )
		vga.d3_panel_up( INFO_X1, dispY1, INFO_X2, dispY1+22 );

	int x=INFO_X1+4, y=dispY1+3;
}
//----------- End of function FirmMonster::disp_monster_info -----------//
*/

