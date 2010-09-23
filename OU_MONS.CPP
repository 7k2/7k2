//Filename   : OU_MONS.CPP
//Description: Unit Monster

#include <OWORLD.H>
#include <OSITE.H>
#include <ONEWS.H>
#include <OTOWN.H>
#include <OFIRM.H>
#include <ONATION.H>
#include <OMONSRES.H>
#include <OU_MONS.H>
#include <OT_UNIT.H>


//--------- Begin of function UnitMonster::unit_name ---------//
//
// [int] withTitle - whether return a string with the title of the unit
//                   or not. (default: 1)
//
// [int] firstNameOnly - whether return the first word of the name only
//							(default: 0)
//
char* UnitMonster::unit_name(int withTitle, int firstNameOnly)
{
	char* monsterName;

	if( rank_id == RANK_KING )		// use the player name
		monsterName = nation_array[nation_recno]->king_name();
	else
		monsterName = monster_res.get_name(name_id);

	if (withTitle)
	{		
		return text_unit.str_unit_titled( race_id, monsterName, rank_id );
	}
	else
	{
		return monsterName;
	}
}
//--------- End of function UnitMonster::unit_name ---------//

