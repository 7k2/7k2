//Filename   : ON_HUMAN.CPP
//Description: Functions for the class NationHuman

#include <ONATION2.H>
#include <OGAME.H>

//--------- Begin of function NationHuman::process_ai --------//

void NationHuman::process_ai()
{
	//------- process parent class AI ------//

	int nationRecno = nation_recno;

	Nation::process_ai();

	if( nation_array.is_deleted(nationRecno) )
		return;

	//------ think about surrender and uniting against a big enemy ------//

	if( info.game_date%60 == nation_recno%60 )
	{
		if( think_surrender() )
			return;

		if( think_unite_against_big_enemy() )
			return;
	}
}
//---------- End of function NationHuman::process_ai --------//


//--------- Begin of function NationHuman::process_ai_main --------//

int NationHuman::process_ai_main(int mainActionId)
{
	//------ stop in here if in tutorial mode -----//
	if( game.game_mode == GAME_TUTORIAL )
		return 0;	

	if( Nation::process_ai_main(mainActionId) )
		return 1;

	return 0;
}
//---------- End of function NationHuman::process_ai_main --------//

