//Filename    : OGAMCON.CPP
//Description : Game ending condition

#include <ALL.H>
#include <OINFO.H>
#include <OGAME.H>
#include <OCONFIG.H>
#include <ODATE.H>

//------ Begin of function Game::reset_win_condition -------//
//
void Game::reset_win_condition()
{
	win_condition_count=0;
}
//------ End of function Game::reset_win_condition -------//


//------ Begin of function Game::add_win_condition -------//
//
// <int> condId 	  - game end condition id.
// <int> playerOnly - whether this condition only applies to the player
// [int] p1     	  - parameter 1
// [int] p2     	  - parameter 2
// [int] yearLimit  - year limit
//
void Game::add_win_condition(int condId, int playerOnly, int p1, int p2, int yearLimit)
{
	err_when( win_condition_count >= MAX_WIN_CONDITION );

	win_condition[win_condition_count].init(condId, playerOnly, p1, p2, yearLimit, 0);

	win_condition_count++;
}
//------ End of function Game::add_win_condition -------//


//------ Begin of function Game::del_win_condition --------//
//
// remove win condition
//
void Game::del_win_condition(int i)
{
	if( i > 0 && i <= win_condition_count )
	{
		m.del_array_rec( win_condition, MAX_WIN_CONDITION, sizeof(win_condition[0]), i );
		--win_condition_count;
	}
}
//------ End of function Game::del_win_condition --------//


//------ Begin of function Game::add_lose_condition -------//
//
// <int> condId 	  - game end condition id.
// <int> playerOnly - whether this condition only applies to the player
// [int] p1     	  - parameter 1
// [int] p2     	  - parameter 2
// [int] yearLimit  - year limit

void Game::add_lose_condition(int condId, int playerOnly, int p1, int p2, int yearLimit)
{
	err_when( lose_condition_count >= MAX_LOSE_CONDITION );

	lose_condition[lose_condition_count].init(condId, playerOnly, p1, p2, yearLimit, 1);

	lose_condition_count++;
}
//------ End of function Game::add_lose_condition -------//


//------ Begin of function Game::del_lose_condition --------//
//
// remove lose condition
//
void Game::del_lose_condition(int i)
{
	if( i > 0 && i <= lose_condition_count )
	{
		m.del_array_rec( lose_condition, MAX_LOSE_CONDITION, sizeof(lose_condition[0]), i );
		--lose_condition_count;
	}
}
//------ End of function Game::del_lose_condition --------//


//------ Begin of function Game::set_win_deadline -------//
//
void Game::set_win_deadline()
{
	int maxTimeLimit=0;

	// ------- maximum of all win condition ------//

	for( int i=0 ; i<game.win_condition_count ; i++ )
	{
		if( game.win_condition[i].time_limit > maxTimeLimit )
		{
			maxTimeLimit = game.win_condition[i].time_limit;
		}
	}

	// ###### begin Gilbert 25/2 ######//
	// ------- see also config.goal_year_limit_flag ------//

	if( !is_campaign_mode() && config.goal_year_limit_flag )
	{
		if( config.goal_year_limit > maxTimeLimit )
		{
			maxTimeLimit = config.goal_year_limit;
		}
	}
	// ###### end Gilbert 25/2 ######//

	if( maxTimeLimit==0 )
		win_deadline = 0;
	else
	{
		win_deadline = date.add_months( info.game_date, 12 * maxTimeLimit);
	}
}
//------ End of function Game::set_win_deadline -------//



// ----- Begin of function Game::update_condition_on_killed ------//
//
int Game::update_condition_on_killed(long uniqueId)
{
	int rc = 0;
	int i;
	for( i = 0; i < win_condition_count; ++i )
	{
		if( win_condition[i].update_condition_on_killed(uniqueId) )
			rc++;
	}
	for( i = 0; i < lose_condition_count; ++i )
	{
		if( lose_condition[i].update_condition_on_killed(uniqueId) )
			rc++;
	}

	return rc;
}
// ----- End of function Game::update_condition_on_killed ------//


// ----- Begin of function Game::update_condition_on_mobilize ------//
//
int Game::update_condition_on_mobilize(int newUnitRecno, long uniqueId)
{
	int rc = 0;
	int i;
	for( i = 0; i < win_condition_count; ++i )
	{
		if( win_condition[i].update_condition_on_mobilize(newUnitRecno, uniqueId) )
			rc++;
	}
	for( i = 0; i < lose_condition_count; ++i )
	{
		if( lose_condition[i].update_condition_on_mobilize(newUnitRecno, uniqueId) )
			rc++;
	}

	return rc;
}
// ----- End of function Game::update_condition_on_mobilize ------//


// ----- Begin of function Game::update_condition_site_taken ------//
//
int Game::update_condition_site_taken(int siteRecno, int nationRecno)
{
	int rc = 0;
	int i;
	for( i = 0; i < win_condition_count; ++i )
	{
		if( win_condition[i].update_condition_site_taken(siteRecno, nationRecno) )
			rc++;
	}
	for( i = 0; i < lose_condition_count; ++i )
	{
		if( lose_condition[i].update_condition_site_taken(siteRecno, nationRecno) )
			rc++;
	}

	return rc;
}
// ----- End of function Game::update_condition_site_taken ------//


// ----- Begin of function Game::update_condition_item_acquired ------//
//
int Game::update_condition_item_acquired(short itemId, int nationRecno)
{
	int rc = 0;
	int i;
	for( i = 0; i < win_condition_count; ++i )
	{
		if( win_condition[i].update_condition_item_acquired(itemId, nationRecno) )
			rc++;
	}
	for( i = 0; i < lose_condition_count; ++i )
	{
		if( lose_condition[i].update_condition_item_acquired(itemId, nationRecno) )
			rc++;
	}

	return rc;
}
// ----- End of function Game::update_condition_site_taken ------//


// ----- Begin of function Game::is_unit_in_condition ------//
//
int Game::is_unit_in_condition(long uniqueId)
{
	int rc = 0;
	int i;
	for( i = 0; i < win_condition_count; ++i )
	{
		if( win_condition[i].is_unit_in_condition(uniqueId) )
			rc++;
	}
	for( i = 0; i < lose_condition_count; ++i )
	{
		if( lose_condition[i].is_unit_in_condition(uniqueId) )
			rc++;
	}

	return rc;
}
// ----- End of function Game::is_unit_in_condition ------//
