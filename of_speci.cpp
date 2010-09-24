// Filename     :OF_SPECI.CPP
// Description  : FirmSpecial interface

#include <OF_SPEC.H>
#include <ALL.H>
#include <OMODEID.H>
#include <OINFO.H>
#include <OBUTT3D.H>
#include <OFONT.H>
#include <ORACERES.H>
#include <OUNITRES.H>
#include <OTOWN.H>
#include <OSE.H>
#include <OSERES.H>

static Button3D button_train;


// ------- begin of function FirmSpecial::put_info --------//
//
void FirmSpecial::put_info(int refreshFlag)
{
	if( !should_show_info() )
		return;
	
	FirmTrain::put_info(refreshFlag);
}
// ------- end of function FirmSpecial::put_info --------//


// ------- begin of function FirmSpecial::detect_info --------//
//
void FirmSpecial::detect_info()
{
	FirmTrain::detect_info();
}
// ------- end of function FirmSpecial::detect_info --------//


// ------- begin of function FirmSpecial::disp_firm_info --------//
//
void FirmSpecial::disp_firm_info(int dispY1, int refreshFlag )
{
	FirmTrain::disp_firm_info(dispY1, refreshFlag);

	// ------- display train button ------//

	if( refreshFlag == INFO_REPAINT )
	{
		if (!is_monster())
			button_train.create( INFO_X1+13, INFO_Y1+281, 'A', "S_TRAIN" );
		else
			button_train.create( INFO_X1+13, INFO_Y1+281, 'A', "F_TRAIN" );

		button_train.enable_flag = 0;
	}

	// ------ display special unit icon and unit name -----//

	int unitId = race_res[race_id]->special_unit_id;
	info.draw_unit_icon( INFO_X1+40, INFO_Y1+52,
		unitId, nation_recno,
		INFO_X1+15, INFO_Y1+5, INFO_X1+68, INFO_Y1+84, 1 );

	font_snds.center_put( INFO_X1+88, INFO_Y1+15, INFO_X1+217, INFO_Y1+74, unit_res[unitId]->name, 0, 0 );

	if( !should_show_info() )
		return;

	if( is_own() )
	{
		button_train.enable_flag = can_recruit(active_link_town_recno, TRAIN_UNIT_SPECIAL);
		button_train.paint();
	}
}
// ------- end of function FirmSpecial::disp_firm_info --------//

// ------- begin of function FirmSpecial::detect_firm_info --------//
//
void FirmSpecial::detect_firm_info()
{
	FirmTrain::detect_firm_info();

	if( !should_show_info() )
		return;

	// ------- detect train button -------//

	int rc = button_train.detect(0, 0, 1) ;

	if( rc )
	{
		cur_train_type = TRAIN_UNIT_SPECIAL;

		if( !town_array.is_deleted(active_link_town_recno) 
			&& town_array[active_link_town_recno]->recruitable_pop(1) > 0)
		{
			recruit_trainee( active_link_town_recno, cur_train_type, COMMAND_PLAYER );
		}
		else
		{
			short townRecnoArray[MAX_RACE];
			int raceCount = scan_best_recruitable_town( townRecnoArray, 0, required_linked_firm[cur_train_type] );

			err_when( raceCount > 1 );		// if race_id is specified, only one town is returned
			if( rc == 1 && raceCount == 1 )
			{
				err_when( !townRecnoArray[0] );
				recruit_trainee(townRecnoArray[0], cur_train_type, COMMAND_PLAYER );
			}
			else if( rc == 2 || rc == 1 && raceCount > 1 )
			{
				firm_menu_mode = FIRM_TRAIN_MENU_TRAIN;
			}
		}
	//	se_ctrl.immediate_sound("TURN_ON");
		se_ctrl.immediate_sound("ARM-ALRT");
	}
}
// ------- end of function FirmSpecial::detect_firm_info --------//


// ------- begin of function FirmSpecial::disp_train_menu ------//
//
void FirmSpecial::disp_train_menu(int refreshFlag)
{
	FirmTrain::disp_train_menu(refreshFlag);
}
// ------- end of function FirmSpecial::disp_train_menu ------//


// ------- begin of function FirmSpecial::detect_train_menu ------//
//
void FirmSpecial::detect_train_menu()
{
	FirmTrain::detect_train_menu();
}
// ------- end of function FirmSpecial::detect_train_menu ------//


// ------- begin of function FirmSpecial::disp_spy_menu ------//
//
void FirmSpecial::disp_spy_menu(int refreshFlag)
{
	err_here();
}
// ------- end of function FirmSpecial::disp_spy_menu ------//


// ------- begin of function FirmSpecial::detect_spy_menu ------//
//
void FirmSpecial::detect_spy_menu()
{
	err_here();
}
// ------- end of function FirmSpecial::detect_spy_menu ------//


// ------- begin of function FirmSpecial::represent_attribute --------//
//
int FirmSpecial::represent_attribute(int traineeId)
{
	
	err_when( traineeId <= 0 || traineeId > trainee_count );
//	return trainee_array[traineeId-1].skill.combat_level;		// don't display show_combat_level
	Trainee *traineePtr = trainee_array + traineeId - 1;
	return traineePtr->combat_level();

}
// ------- end of function FirmSpecial::represent_attribute --------//

