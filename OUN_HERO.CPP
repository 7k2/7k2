//Filename   : OUN_HERO.CPP
//Description: Unit functions related to heroes.

#include <OUNIT.H>
#include <ONATION.H>
#include <OHERORES.H>


//------ Begin of function Unit::is_hero_for_hire -------//
//
// Return whether this hero is for hire or not.
//
int Unit::is_hero_for_hire()
{
	return nation_recno==0 &&
			 hero_id &&
			 hero_res[hero_id]->for_hire;
}
//------- End of function Unit::is_hero_for_hire -------//


//------ Begin of function Unit::hero_hire_cost -------//

int Unit::hero_hire_cost()
{
	return hero_res.hire_cost( combat_level(), skill.max_combat_level,
										skill_level(), 0, &item );
}
//------- End of function Unit::hero_hire_cost -------//


//------ Begin of function Unit::can_hire_hero -------//
//
// Return whether the given nation can hire this hero.
//
int Unit::can_hire_hero(int nationRecno)
{
	return is_hero_for_hire() &&
			 nation_array[nationRecno]->is_human() &&						// only human nations can hire heroes
			 nation_array[nationRecno]->cash >= hero_hire_cost();
}

//------- End of function Unit::can_hire_hero -------//


//------ Begin of function Unit::hire_hero -------//
//
// <int> nationRecno - the recno of the nation to hire this hero.
//
int Unit::hire_hero(int nationRecno)
{
	if( !can_hire_hero(nationRecno) )
	{
		if( unit_array.selected_recno == sprite_recno
			&& nationRecno == nation_array.player_recno )
		{
			hire_hero_result = -1;
		}
		return 0;
	}

	//------- pay money -------//

	nation_array[nationRecno]->add_expense( EXPENSE_HIRE_UNIT, (float) hero_hire_cost() );

	betray(nationRecno);
	stop_order();
	set_rank(RANK_GENERAL);		// automatically promote him to general

	// ###### begin Gilbert 4/3 #########//

	// ------ set hire_hero_result -------//

	if( unit_array.selected_recno == sprite_recno
		&& nationRecno == nation_array.player_recno )
	{
		hire_hero_result = 1;
	}
	// ###### end Gilbert 4/3 #########//

	return 1;
}
//------- End of function Unit::hire_hero -------//

