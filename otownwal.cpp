// Filename    : OTOWNWAL.CPP
// Description : Town wall


#include <OTOWN.H>
#include <OCONFIG.H>
#include <OREMOTE.H>
#include <OINFO.H>
#include <ONATION.H>
#include <OGODRES.H>


//-------- begin of function Town::process_wall ----------//
//
// return number of peasant assign to build wall
//
int Town::process_wall()
{
	if( info.game_date <= last_attack_date+4 )
		return 0;

	// ###### begin Gilbert 14/4 #######//
	// ------- automatically get max level wall ---------//

	int godsWall;
	if( nation_recno && god_res[GOD_GREEK]->nation_prayer_count(nation_recno) > 0 )
		godsWall = 1;
	else
		godsWall = 0;

	if( godsWall )
	{
		if( target_wall_level < MAX_WALL_LEVEL )		// avoid promote to max wall level repetitive
		{
			set_target_wall_level( MAX_WALL_LEVEL, COMMAND_AUTO );	// grant in process_wall as town may appear before the seat of power
		}
	}
	// ###### end Gilbert 14/4 #######//

	err_when( target_wall_man_power < 0 );

	int actualManPower = target_wall_man_power;
	if( actualManPower > jobless_population )
		actualManPower = jobless_population;

	// when a wall is destoryed
	if( current_wall_level > 0 && hit_points <= 0.0f )
	{
		current_wall_level = 0;
		place_max_hit_points = hit_points = 0.0f;
		if (target_wall_level > 1)
			target_wall_level = 1;
	}
	else if( hit_points < max_hit_points() )
	{
		// god's repair

		if( hit_points < max_hit_points() && godsWall )
		{
			hit_points += REPAIR_POINTS_PER_DAY;
		}

		// repair but cost money

		if( hit_points < max_hit_points() && repair_flag && actualManPower > 0
			&& (!nation_recno || nation_array[nation_recno]->cash > 0) )
		{
			hit_points += actualManPower * REPAIR_POINTS_PER_DAY;

			//------- add expense, no cast if god assist ---------//

			if( nation_recno && actualManPower > 0 )
			{
				int wallCost;

				if( current_wall_level == 1 )
					wallCost = LEVEL_ONE_WALL_COST;
				else if( current_wall_level == 2 )
					wallCost = LEVEL_TWO_WALL_COST;
				else
					err_here();

				float expenseAmt = (float) wallCost * ((float) actualManPower * REPAIR_POINTS_PER_DAY) / max_hit_points();

				err_when( expenseAmt < 0 );

				nation_array[nation_recno]->add_expense( EXPENSE_TOWN, expenseAmt, 0 );
			}
		}
		else
		{
			actualManPower = 0;		// no repair, so no man power used
		}

		if( hit_points >= max_hit_points() )
		{
			hit_points = (float) max_hit_points();
			set_repair_wall( 0, COMMAND_AUTO );
		}
	
		return actualManPower;
	}
	else if( current_wall_level < target_wall_level )
	{
		// full hit points, consider upgrade wall

		int thisProgress = 0;
		if( godsWall )
		{
			if( config.fast_build && is_own() )
				thisProgress += 80;
			else
				thisProgress += 20;		// god build wall faster

			if( actualManPower == 1 )		// since target_wall_man_power is always 1, disable manual when god happen
				actualManPower = 0;			// 
		}

		// upgrade wall by man

		if( actualManPower > 0 && (!nation_recno || nation_array[nation_recno]->cash > 0) )
		{
			if( config.fast_build && is_own() )
				thisProgress += actualManPower * 40;
			else
				thisProgress += actualManPower * 10;

			//------- add expense ---------//

			if( nation_recno )
			{
				Nation* nationPtr = nation_array[nation_recno];
				int	  wallCost;

				if( current_wall_level == 0 )
					wallCost = LEVEL_ONE_WALL_COST;
				else if( current_wall_level == 1 )
					wallCost = LEVEL_TWO_WALL_COST;
				else
					err_here();

				float expenseAmt = (float) wallCost * actualManPower * 10 / MAX_WALL_PROGRESS;
				err_when( expenseAmt < 0 );
				nationPtr->add_expense( EXPENSE_TOWN, expenseAmt, 0 );
			}
		}
		else
		{
			actualManPower = 0;
		}

		upgrade_wall_progress += thisProgress;

		if( upgrade_wall_progress >= MAX_WALL_PROGRESS )
		{
			current_wall_level++;
			place_max_hit_points = current_wall_level * 100.0f;
			hit_points += 100.0f;
			err_when( hit_points > max_hit_points() );
			upgrade_wall_progress -= MAX_WALL_PROGRESS;
		}

		return actualManPower;
	}

	return 0;
}
//-------- end of function Town::process_wall ----------//


//-------- begin of function Town::set_repair_wall ----------//
//
void Town::set_repair_wall(int repairFlag, char remoteAction)
{
	if( !remoteAction && remote.is_enable() )
	{
		// ##### begin Gilbert 7/10 ########//
		// packet structure : <town recno> <new repair flag>
		short *shortPtr = (short *)remote.new_send_queue_msg(MSG_TOWN_SET_REPAIR, 2*sizeof(short) );
		*shortPtr = town_recno;
		shortPtr[1] = repairFlag;
		// ##### end Gilbert 7/10 ########//
		return;
	}
	repair_flag = repairFlag;
}
//-------- end of function Town::set_repair_wall ----------//


//-------- begin of function Town::set_current_wall_level ----------//
//
void Town::set_current_wall_level(int newLevel)
{
	current_wall_level = target_wall_level = newLevel;

	place_max_hit_points = hit_points = current_wall_level * 100.0f;
}
//-------- end of function Town::set_current_wall_level ----------//


//-------- begin of function Town::set_target_wall_level ----------//
//
void Town::set_target_wall_level(int newLevel, char remoteAction)
{
	err_when( newLevel < 0 || newLevel > MAX_WALL_LEVEL );

	if( newLevel < current_wall_level )		// target_wall_level never below current_wall_level
		return;

	if( !remoteAction && remote.is_enable() )
	{
		short *shortPtr = (short *)remote.new_send_queue_msg(MSG_TOWN_TARGET_WALL, 2*sizeof(short) );
		*shortPtr = town_recno;
		shortPtr[1] = newLevel;
		return;
	}

	target_wall_level = newLevel;
}
//-------- end of function Town::set_target_wall_level ----------//
