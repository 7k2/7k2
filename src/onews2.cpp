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

//Filename    : ONEWS2.CPP
//Description : News adding functions

#include <otown.h>
#include <onation.h>
#include <oherores.h>
#include <oraceres.h>
#include <omonsres.h>
#include <otalkres.h>
#include <ofirm.h>
#include <ospy.h>
#include <ounit.h>
#include <otornado.h>
#include <onews.h>
#include <ose.h> 

//-------- Declare static functions ---------//

static int news_unit_name_id(Unit* unitPtr);
static int news_unit_name_id(Spy *spyPtr);

//------ Begin of function NewsArray::diplomacy -----//
//
// <int> talkMsgRecno = the recno of the TalkMsg in talk_res.talk_msg_array
//
// short_para1 = the recno of the TalkMsg in talk_res.talk_msg_array
//
int NewsArray::diplomacy(int talkMsgRecno)
{
	//----------- add news --------------//

	err_when( talk_res.is_talk_msg_deleted(talkMsgRecno) );

	TalkMsg* talkMsgPtr = talk_res.get_talk_msg(talkMsgRecno);

	News* newsPtr = add_news( NEWS_DIPLOMACY, NEWS_NORMAL, talkMsgPtr->from_nation_recno, talkMsgPtr->to_nation_recno );

	if( !newsPtr )		// only news of nations that have contact with the player are added
		return 0;

	newsPtr->short_para1 = talkMsgRecno;

	return 1;
}
//------- End of function NewsArray::diplomacy -----//

//------ Begin of function NewsArray::town_rebel -----//
//
// <int> townRecno  = the recno of the town where people there rebel
// <int> rebelCount = no. of rebels
//
// short_para1 = the town name id.
// short_para2 = no. of rebels
//
void NewsArray::town_rebel(int townRecno, int rebelCount)
{
	Town* townPtr = town_array[townRecno];

	//----------- add news --------------//

	News* newsPtr = add_news( NEWS_TOWN_REBEL, NEWS_NORMAL, townPtr->nation_recno );

	if( !newsPtr )		// only news of nations that have contact with the player are added
		return;

	newsPtr->short_para1 = townPtr->town_name_id;
	newsPtr->short_para2 = rebelCount;

	//-------- set location ----------//

	newsPtr->set_loc( townPtr->center_x, townPtr->center_y, NEWS_LOC_TOWN, townRecno );
}
//------- End of function NewsArray::town_rebel -----//


//------ Begin of function NewsArray::migrate -----//
//
// <int> srcTownRecno = the town that the worker migrates from
// <int> desTownRecno = the town that the worker migrates to
// <int> raceId		 = race id. of the migrated worker/peasant
// <int> migratedCount= no. of people migrated.
// [int] firmRecno    = if firm worker migrates,
//					  			>this is the firm id. that the worker works for
//               			if town peasant migrates
//					  			>this is 0
//
// short_para1 = the town name id. that the worker migrates from
// short_para2 = the town name id. that the worker migrates to
// short_para3 = race id. of the migrated worker/peasant
// short_para4 = no. of people migrated
// short_para5 = the firm id. that the worker works for
//
void NewsArray::migrate(int srcTownRecno, int desTownRecno, int raceId, int migratedCount, int firmRecno)
{
	err_when( srcTownRecno==0 || desTownRecno==0 || raceId==0 || migratedCount==0 );

	Town* srcTown = town_array[srcTownRecno];
	Town* desTown = town_array[desTownRecno];

	//----------- add news --------------//

	News* newsPtr = add_news( NEWS_MIGRATE, NEWS_NORMAL, srcTown->nation_recno, desTown->nation_recno );

	if( !newsPtr )		// only news of nations that have contact with the player are added
		return;

	newsPtr->short_para1 = srcTown->town_name_id;
	newsPtr->short_para2 = desTown->town_name_id;
	newsPtr->short_para3 = raceId;
	newsPtr->short_para4 = migratedCount;

	if( firmRecno )
		newsPtr->short_para5 = firm_array[firmRecno]->firm_id;
	else
		newsPtr->short_para5 = 0;

	//-------- set location ----------//

	newsPtr->set_loc( desTown->center_x, desTown->center_y, NEWS_LOC_TOWN, desTownRecno );
}
//------- End of function NewsArray::migrate -----//


//------ Begin of function NewsArray::new_nation -----//
//
// <int> nationRecno - recno of the new nation.
//
// king_name1() - name of the king of the new kingdom.
//
void NewsArray::new_nation(int nationRecno)
{
	News* newsPtr = add_news( NEWS_NEW_NATION, NEWS_NORMAL, nationRecno );

	if( !newsPtr )
		return;

	//---- set the news location to one of its town ----//

	for( int i=town_array.size() ; i>0 ; i-- )
	{
		if( town_array.is_deleted(i) )
			continue;

		Town* townPtr = town_array[i];

		if( townPtr->nation_recno == nationRecno )
		{
			newsPtr->set_loc( townPtr->center_x, townPtr->center_y,
									NEWS_LOC_TOWN, i );
			break;
		}
	}
}
//------- End of function NewsArray::new_nation -----//


//------ Begin of function NewsArray::nation_destroyed -----//
//
// <int> nationRecno - recno of the nation that has been destroyed
//
// nation_name1() - the nation that has been destroyed.
//
void NewsArray::nation_destroyed(int nationRecno)
{
	add_news( NEWS_NATION_DESTROYED, NEWS_NORMAL, nationRecno );
}
//------- End of function NewsArray::nation_destroyed -----//


//------ Begin of function NewsArray::nation_surrender -----//
//
// <int> nationRecno   - recno of the surrendering nation.
// <int> toNationRecno - recno of the nation to surrender.
//
void NewsArray::nation_surrender(int nationRecno, int toNationRecno)
{
	add_news( NEWS_NATION_SURRENDER, NEWS_NORMAL, nationRecno, toNationRecno );
}
//------- End of function NewsArray::nation_surrender -----//


//------ Begin of function NewsArray::king_die -----//
//
// <int> nationRecno - recno of the nation with its king died.
//
// king_name1() - the nation whose king has died.
//
void NewsArray::king_die(int nationRecno)
{
	add_news( NEWS_KING_DIE, NEWS_NORMAL, nationRecno );
}
//------- End of function NewsArray::king_die -----//


//------ Begin of function NewsArray::new_king -----//
//
// <int> nationRecno   - recno of the nation
// <int> kingUnitRecno - unit recno of the new king.
//
// This function should be called before set_king() has been called.
//
// nation_name1() - name of the nation where there is a new king.
//
// short_para1 - race id. of the new king.
// short_para2 - name id. of the new king.
//
void NewsArray::new_king(int nationRecno, int kingUnitRecno)
{
	News* newsPtr = add_news( NEWS_NEW_KING, NEWS_NORMAL, nationRecno );

	if( !newsPtr )
		return;

	Unit* unitPtr = unit_array[kingUnitRecno];

	newsPtr->short_para1 = unitPtr->race_id;
	newsPtr->short_para2 = news_unit_name_id(unitPtr);
}
//------- End of function NewsArray::new_king -----//


//------ Begin of function NewsArray::firm_destroyed -----//
//
// <int>   	  firmRecno - recno of the firm destroyed.
// <BaseObj*> attackObj - BaseObj recno to the attacking object.
//
// short_para1 - build id. of the firm destroyed.
// short_para2 - name id of the town where the firm is located.
// short_para3 - destroyer type: 1 - a nation, 2 - rebels, 3 - Fryhtans.
//
void NewsArray::firm_destroyed(int firmRecno, BaseObj* attackObj)
{
	Firm* firmPtr = firm_array[firmRecno];

	err_when( firmPtr->nation_recno != nation_array.player_recno );

	int destroyerNationRecno=0;

	if( attackObj )
		destroyerNationRecno = attackObj->nation_recno;

	News* newsPtr = add_news( NEWS_FIRM_DESTROYED, NEWS_NORMAL, firmPtr->nation_recno, destroyerNationRecno );

	if( !newsPtr )		// only news of nations that have contact with the player are added
		return;

	newsPtr->short_para1 = firmPtr->firm_build_id;

	if( firmPtr->closest_town_name_id )
		newsPtr->short_para2 = firmPtr->closest_town_name_id;
	else
		newsPtr->short_para2 = firmPtr->get_closest_town_name_id();

	//-------- set destroyer type ------//

	newsPtr->short_para3 = DESTROYER_UNKNOWN;

	// ####### begin Gilbert 17/6 ########//
	/*
	if( attackObj )
	{
		if (attackObj->cast_to_Town())	//destroy by town
			newsPtr->short_para3 = DESTROYER_UNKNOWN;
		
		else
		if (attackObj->cast_to_Firm())
			newsPtr->short_para3 = DESTROYER_UNKNOWN;
		
		else
		{
			newsPtr->short_para3 = DESTROYER_NATION;

			Unit* attackUnit = attackObj->cast_to_Unit();
	
			if( attackUnit )
			{
				if( attackUnit->unit_mode == UNIT_MODE_REBEL )
					newsPtr->short_para3 = DESTROYER_REBEL;
				else
					newsPtr->short_para3 = DESTROYER_UNKNOWN;
			}
			else
				err_here();// being destoryed by non-unit
		}
	}
	*/

	if( attackObj )
	{
		if( attackObj->nation_recno )
		{
			newsPtr->short_para3 = DESTROYER_NATION;
		}
		else if( attackObj->cast_to_Unit() )
		{
			Unit* attackUnit = attackObj->cast_to_Unit();
			if( attackUnit->unit_mode == UNIT_MODE_REBEL )
				newsPtr->short_para3 = DESTROYER_REBEL;
			else if( attackUnit->is_monster() )
				newsPtr->short_para3 = DESTROYER_MONSTER;
		}
	}

	// ####### end Gilbert 17/6 ########//

	//--------- set location ---------//

	newsPtr->set_loc( firmPtr->center_x, firmPtr->center_y, NEWS_LOC_ANY );
}
//------- End of function NewsArray::firm_destroyed -----//

//------- Begin of function NewsArray::firm_attacked -----//
//
// <int>   	  firmRecno - recno of the firm attacked.
// <BaseObj*> attackObj - BaseObj recno to the attacking object.
//
// short_para1 - build id. of the firm destroyed.
// short_para2 - name id of the town where the firm is located.
// short_para3 - destroyer type: 1 - a nation, 2 - rebels, 3 - Fryhtans.
//
void NewsArray::firm_attacked(int firmRecno, BaseObj* attackObj)
{
	Firm* firmPtr = firm_array[firmRecno];

	err_when( firmPtr->nation_recno != nation_array.player_recno );

	int destroyerNationRecno=0;

	if( attackObj )
		destroyerNationRecno = attackObj->nation_recno;

	News* newsPtr = add_news( NEWS_FIRM_ATTACKED, NEWS_NORMAL, firmPtr->nation_recno, destroyerNationRecno );

	if( !newsPtr )		// only news of nations that have contact with the player are added
		return;

	newsPtr->short_para1 = firmPtr->firm_build_id;

	if( firmPtr->closest_town_name_id )
		newsPtr->short_para2 = firmPtr->closest_town_name_id;
	else
		newsPtr->short_para2 = firmPtr->get_closest_town_name_id();

	//-------- set destroyer type ------//

	newsPtr->short_para3 = DESTROYER_UNKNOWN;

	// ###### begin Gilbert 17/6 ########//
/*
	if( attackObj )
	{
		if (attackObj->cast_to_Town())	//destroy by town
			newsPtr->short_para3 = DESTROYER_UNKNOWN;
		
		else
		if (attackObj->cast_to_Firm())
			newsPtr->short_para3 = DESTROYER_UNKNOWN;
		
		else
		{
			newsPtr->short_para3 = DESTROYER_NATION;

			Unit* attackUnit = attackObj->cast_to_Unit();
	
			if( attackUnit )
			{
				if( attackUnit->unit_mode == UNIT_MODE_REBEL )
					newsPtr->short_para3 = DESTROYER_REBEL;
				else
					newsPtr->short_para3 = DESTROYER_UNKNOWN;
			}
			else
				err_here();// being destoryed by non-unit
		}
	}
*/

	if( attackObj )
	{
		if( attackObj->nation_recno )
		{
			newsPtr->short_para3 = DESTROYER_NATION;
		}
		else if( attackObj->cast_to_Unit() )
		{
			Unit* attackUnit = attackObj->cast_to_Unit();
			if( attackUnit->unit_mode == UNIT_MODE_REBEL )
				newsPtr->short_para3 = DESTROYER_REBEL;
			else if( attackUnit->is_monster() )
				newsPtr->short_para3 = DESTROYER_MONSTER;
		}
	}
	// ###### end Gilbert 17/6 ########//

	//--------- set location ---------//

	newsPtr->set_loc( firmPtr->center_x, firmPtr->center_y, NEWS_LOC_ANY );
}
//------- End of function NewsArray::firm_attacked -----//

//------ Begin of function NewsArray::firm_captured -----//
//
// <int> firmRecno 	  - recno of the firm destroyed.
// <int> takeoverNationRecno - recno of nation that has taken over the firm.
// <int> spyTakeover	  - whether the capturer of the firm is a spy.
//
// short_para1 - firm build id. of the firm.
// short_para2 - name id of the town where the firm is located.
// short_para3 - whether the capturer of the firm is a spy.
//
void NewsArray::firm_captured(int firmRecno, int takeoverNationRecno, int spyTakeover)
{
	Firm* firmPtr = firm_array[firmRecno];

	err_when( firmPtr->nation_recno != nation_array.player_recno );

	News* newsPtr = add_news( NEWS_FIRM_CAPTURED, NEWS_NORMAL, firmPtr->nation_recno, takeoverNationRecno );

	if( !newsPtr )		// only news of nations that have contact with the player are added
		return;

	newsPtr->short_para1 = firmPtr->firm_build_id;

	if( firmPtr->closest_town_name_id )
		newsPtr->short_para2 = firmPtr->closest_town_name_id;
	else
		newsPtr->short_para2 = firmPtr->get_closest_town_name_id();

	newsPtr->short_para3 = spyTakeover;

	//--------- set location ---------//

	newsPtr->set_loc( firmPtr->center_x, firmPtr->center_y, NEWS_LOC_FIRM, firmRecno );
}
//------- End of function NewsArray::firm_captured -----//


//------ Begin of function NewsArray::town_destroyed -----//
//
// <char*> townName   - name of the town destroyed.
// <int> xLoc, yLoc   - location of the town
// <BaseObj*> attackerObj - recno to the attacking unit.
//
// short_para1 - name id. of the town destroyed.
// short_para2 - destroyer type: 1 - a nation, 2 - rebels, 3 - Fryhtans.
//
void NewsArray::town_destroyed(char *townName, int xLoc, int yLoc, BaseObj* attackerObj)
{
	int destroyerNationRecno=0;

	if( attackerObj )
		destroyerNationRecno = attackerObj->nation_recno;

	News* newsPtr = add_news( NEWS_TOWN_DESTROYED, NEWS_NORMAL, nation_array.player_recno, destroyerNationRecno );

	if( !newsPtr )		// only news of nations that have contact with the player are added
		return;

	// newsPtr->short_para1 = townNameId;
	m.str_cut( newsPtr->str_para1, townName, 1, NEWS_STR_PARA_LEN );

	//-------- set destroyer type ------//

	newsPtr->short_para2 = DESTROYER_UNKNOWN;

	// ######### begin Gilbert 17/6 ########//
/*
	if( attackerObj )
	{
		if (attackerObj->cast_to_Town())	//destroy by town
			newsPtr->short_para2 = DESTROYER_UNKNOWN;
		
		else
		if (attackerObj->cast_to_Firm())
			newsPtr->short_para2 = DESTROYER_UNKNOWN;
		
		else
		{
			newsPtr->short_para2 = DESTROYER_NATION;
		
			Unit* attackerUnit = attackerObj->cast_to_Unit();

			if( attackerUnit )
			{
				if( attackerUnit->unit_mode == UNIT_MODE_REBEL )
					newsPtr->short_para2 = DESTROYER_REBEL;
			}
		}
	}
*/
	if( attackerObj )
	{
		if( attackerObj->nation_recno )
		{
			newsPtr->short_para2 = DESTROYER_NATION;
		}
		else if( attackerObj->cast_to_Unit() )
		{
			Unit* attackUnit = attackerObj->cast_to_Unit();
			if( attackUnit->unit_mode == UNIT_MODE_REBEL )
				newsPtr->short_para2 = DESTROYER_REBEL;
			else if( attackUnit->is_monster() )
				newsPtr->short_para2 = DESTROYER_MONSTER;
		}
	}
	// ###### end Gilbert 17/6 ########//

	//-------- set location ----------//

	newsPtr->set_loc( xLoc, yLoc, NEWS_LOC_ANY );
}
//------- End of function NewsArray::town_destroyed -----//

//------ Begin of function NewsArray::town_attacked -----//
//
// <char*> townName   - name of the town attacked.
// <int> xLoc, yLoc   - location of the town
// <BaseObj*> attackerObj - recno to the attacking unit.
//
// short_para1 - name id. of the town attacked.
// short_para2 - destroyer type: 1 - a nation, 2 - rebels, 3 - Fryhtans.
//
void NewsArray::town_attacked(char *townName, int xLoc, int yLoc, BaseObj* attackerObj)
{
	int destroyerNationRecno=0;

	if( attackerObj )
		destroyerNationRecno = attackerObj->nation_recno;

	News* newsPtr = add_news( NEWS_TOWN_ATTACKED, NEWS_NORMAL, nation_array.player_recno, destroyerNationRecno );

	if( !newsPtr )		// only news of nations that have contact with the player are added
		return;

	// newsPtr->short_para1 = townNameId;
	m.str_cut( newsPtr->str_para1, townName, 1, NEWS_STR_PARA_LEN );

	//-------- set destroyer type ------//

	newsPtr->short_para2 = DESTROYER_UNKNOWN;

	// ####### begin Gilbert 17/6 ########//
/*
	if( attackerObj )
	{
		if (attackerObj->cast_to_Town())	//destroy by town
			newsPtr->short_para2 = DESTROYER_UNKNOWN;
		
		else
		if (attackerObj->cast_to_Firm())
			newsPtr->short_para2 = DESTROYER_UNKNOWN;
		
		else
		{
			newsPtr->short_para2 = DESTROYER_NATION;
		
			Unit* attackerUnit = attackerObj->cast_to_Unit();

			if( attackerUnit )
			{
				if( attackerUnit->unit_mode == UNIT_MODE_REBEL )
					newsPtr->short_para2 = DESTROYER_REBEL;
			}
		}
	}
*/

	if( attackerObj )
	{
		if( attackerObj->nation_recno )
		{
			newsPtr->short_para2 = DESTROYER_NATION;
		}
		else if( attackerObj->cast_to_Unit() )
		{
			Unit* attackUnit = attackerObj->cast_to_Unit();
			if( attackUnit->unit_mode == UNIT_MODE_REBEL )
				newsPtr->short_para2 = DESTROYER_REBEL;
			else if( attackUnit->is_monster() )
				newsPtr->short_para2 = DESTROYER_MONSTER;
		}
	}
	// ###### end Gilbert 17/6 ########//

	//-------- set location ----------//

	newsPtr->set_loc( xLoc, yLoc, NEWS_LOC_ANY );
}
//------- End of function NewsArray::town_attacked -----//

//------ Begin of function NewsArray::town_abandoned -----//
//
// <int> townRecno - recno of the town destroyed.
//
// short_para1 - name id. of the town destroyed.
//
void NewsArray::town_abandoned(int townRecno)
{
	Town* townPtr = town_array[townRecno];

	err_when( townPtr->nation_recno != nation_array.player_recno );

	News* newsPtr = add_news( NEWS_TOWN_ABANDONED, NEWS_NORMAL, townPtr->nation_recno );

	if( !newsPtr )		// only news of nations that have contact with the player are added
		return;

	newsPtr->short_para1 = townPtr->town_name_id;

	//-------- set location ----------//

	newsPtr->set_loc( townPtr->center_x, townPtr->center_y, NEWS_LOC_ANY );
}
//------- End of function NewsArray::town_abandoned -----//


//------ Begin of function NewsArray::town_surrendered -----//
//
// <int> townRecno 	  - recno of the town
// <int> toNationRecno - recno of the nation this town surrenders to
//
// short_para1 - name id. of the surrendering town.
//
// nation_name1() - name of the nation the town surrenders to.
// nation_name2() - name of the nation of the surrendering town.
//
// This function should be called before the town surrenders.
//
void NewsArray::town_surrendered(int townRecno, int toNationRecno)
{
	Town* townPtr = town_array[townRecno];

	err_when( townPtr->nation_recno != nation_array.player_recno &&
				 toNationRecno != nation_array.player_recno );

	News* newsPtr = add_news( NEWS_TOWN_SURRENDERED, NEWS_NORMAL, toNationRecno, townPtr->nation_recno );

	if( !newsPtr )		// only news of nations that have contact with the player are added
		return;

	newsPtr->short_para1 = townPtr->town_name_id;

	//-------- set location ----------//

	newsPtr->set_loc( townPtr->center_x, townPtr->center_y, NEWS_LOC_TOWN, townRecno );
}
//------- End of function NewsArray::town_surrendered -----//

//------- Begin of function NewsArray::firm_idle -----//
//
// <int>   	  firmRecno - recno of the seat of power idle.
//
// short_para1 - build id. of the idle seat of power.
// short_para2 - name id of the town where the firm is located.
//
void NewsArray::firm_idle(int firmRecno)
{
	Firm* firmPtr = firm_array[firmRecno];

	err_when( firmPtr->nation_recno != nation_array.player_recno );

	int destroyerNationRecno=0;

	News* newsPtr = add_news( NEWS_FIRM_IDLE, NEWS_NORMAL, firmPtr->nation_recno );

	if( !newsPtr )		// only news of nations that have contact with the player are added
		return;

	newsPtr->short_para1 = firmPtr->firm_build_id;

	if( firmPtr->closest_town_name_id )
		newsPtr->short_para2 = firmPtr->closest_town_name_id;
	else
		newsPtr->short_para2 = firmPtr->get_closest_town_name_id();

	//--------- set location ---------//

	newsPtr->set_loc( firmPtr->center_x, firmPtr->center_y, NEWS_LOC_ANY );
}
//------- End of function NewsArray::firm_idle -----//

//------ Begin of function NewsArray::monster_king_killed -----//
//
// <int> monsterId - monster id. of the monster king.
//
// short_para1 - monster id.
//
void NewsArray::monster_king_killed(int monsterId, int xLoc, int yLoc)
{
	err_when( monsterId < 1 || monsterId > monster_res.monster_count );

	News* newsPtr = add_news( NEWS_MONSTER_KING_KILLED, NEWS_NORMAL );

	if( !newsPtr )		// only news of nations that have contact with the player are added
		return;

	newsPtr->short_para1 = monsterId;

	//-------- set location ----------//

	newsPtr->set_loc( xLoc, yLoc, NEWS_LOC_ANY );
}
//------- End of function NewsArray::monster_king_killed -----//


//------ Begin of function NewsArray::monster_firm_destroyed -----//
//
// <int> monsterId - monster id. of the monster king.
//
// short_para1 - monster id.
//
void NewsArray::monster_firm_destroyed(int monsterId, int xLoc, int yLoc)
{
	err_when( monsterId < 1 || monsterId > monster_res.monster_count );

	News* newsPtr = add_news( NEWS_MONSTER_FIRM_DESTROYED, NEWS_NORMAL );

	if( !newsPtr )		// only news of nations that have contact with the player are added
		return;

	newsPtr->short_para1 = monsterId;

	//-------- set location ----------//

	newsPtr->set_loc( xLoc, yLoc, NEWS_LOC_ANY );
}
//------- End of function NewsArray::monster_firm_destroyed -----//


//------ Begin of function NewsArray::scroll_acquired -----//
//
// <int> acquireNationRecno - recno of the nation that has acquired the scroll
// <int> scrollRaceId	    - race of the scroll
//
// nation_name1() - the nation that has acquired the scroll.
//
// short_para1 = the race id. of the scroll.
//
void NewsArray::scroll_acquired(int acquireNationRecno, int scrollRaceId)
{
	News* newsPtr = add_news( NEWS_SCROLL_ACQUIRED, NEWS_NORMAL, acquireNationRecno );

	if( !newsPtr )		// only news of nations that have contact with the player are added
		return;

	newsPtr->short_para1 = scrollRaceId;
}
//------- End of function NewsArray::scroll_acquired -----//


//------ Begin of function NewsArray::monster_gold_acquired -----//
//
// When the player recovered treasures from monsters.
//
// <int> goldAmt - the amount of treasure recovered.
//
// short_para1 = amount of gold.
//
void NewsArray::monster_gold_acquired(int goldAmt)
{
	News* newsPtr = add_news( NEWS_MONSTER_GOLD_ACQUIRED, NEWS_NORMAL, nation_array.player_recno );

	if( !newsPtr )
		return;

	newsPtr->short_para1 = goldAmt;
}
//------- End of function NewsArray::monster_gold_acquired -----//


//------ Begin of function NewsArray::spy_killed -----//
//
// <int> spyRecno - recno of the spy.
//
// nation_name1() - your nation.
// nation_name2() - the nation that killed your spy.
//
// short_para1 - firm build id. if it's a firm
//					  0 if it's a town
// short_para2 - the town id.
// short_para3 - spy place
//
// This function should be called just right before the spy is killed.
//
void NewsArray::spy_killed(int spyRecno)
{
	Spy*  spyPtr = spy_array[spyRecno];
	News* newsPtr;

	//---------- your spy is killed in an enemy nation ---------//

	if( spyPtr->true_nation_recno == nation_array.player_recno )
	{
		newsPtr = add_news( NEWS_YOUR_SPY_KILLED, NEWS_NORMAL,
					 nation_array.player_recno, spyPtr->cloaked_nation_recno );
	}
	else //----- an enemy spy in your nation is uncovered and executed ----//
	{
		err_when( spyPtr->cloaked_nation_recno != nation_array.player_recno );

		newsPtr = add_news( NEWS_ENEMY_SPY_KILLED, NEWS_NORMAL,
					 nation_array.player_recno, spyPtr->true_nation_recno );
	}

	if( !newsPtr )		// only news of nations that have contact with the player are added
		return;

	//-------------------------------------------//

	newsPtr->short_para3 = spyPtr->spy_place;

	if( spyPtr->spy_place == SPY_FIRM )
	{
		Firm* firmPtr = firm_array[spyPtr->spy_place_para];

		newsPtr->short_para1 = firmPtr->firm_build_id;
		newsPtr->short_para2 = firmPtr->get_closest_town_name_id();

		newsPtr->set_loc( firmPtr->center_x, firmPtr->center_y, NEWS_LOC_FIRM, firmPtr->firm_recno );
	}
	else if( spyPtr->spy_place == SPY_TOWN )
	{
		Town* townPtr = town_array[spyPtr->spy_place_para];

		newsPtr->short_para1 = 0;
		newsPtr->short_para2 = townPtr->town_name_id;

		newsPtr->set_loc( townPtr->center_x, townPtr->center_y, NEWS_LOC_TOWN, townPtr->town_recno );
	}
	else 
	{
		newsPtr->short_para1 = spyPtr->race_id;
		newsPtr->short_para2 = news_unit_name_id(spyPtr);
		int xLoc, yLoc;
		if( spyPtr->get_loc(xLoc, yLoc) )
		{
			newsPtr->set_loc( xLoc, yLoc, NEWS_LOC_ANY );
		}

		if( spyPtr->spy_place == SPY_MOBILE )
		{
			Unit* unitPtr = unit_array[spyPtr->spy_place_para];

			newsPtr->short_para1 = unitPtr->race_id;
			newsPtr->short_para2 = news_unit_name_id(unitPtr);
		}
		else if( spyPtr->spy_place == SPY_WAGON )
		{
			Unit* unitPtr = unit_array[spyPtr->spy_place_para];
			if( unitPtr->is_visible() )
				newsPtr->set_loc( unitPtr->next_x_loc(), unitPtr->next_y_loc(), NEWS_LOC_UNIT, unitPtr->sprite_recno, unitPtr->name_id );
		}
	}
}
//------- End of function NewsArray::spy_killed -----//

//------ Begin of function NewsArray::unit_betray -----//
//
// <int> unitRecno - recno of the unit.
// <int> betrayToNationRecno - the nation which the unit betray towards
//
// nation_name1() - the nation that the unit originally belongs to.
// nation_name2() - the nation that the unit has turned towards.
//
// short_para1 - race id. of the unit
// short_para2 - name id. of the unit
// short_para3 - rank id. of the unit
// short_para4 - unit id. of the unit
//
// This function should be called before the unit betray.
//
void NewsArray::unit_betray(int unitRecno, int betrayToNationRecno)
{
	Unit* unitPtr = unit_array[unitRecno];

	err_when( unitPtr->nation_recno != nation_array.player_recno &&
				 betrayToNationRecno != nation_array.player_recno );

	if (unitPtr->hero_id)
		se_ctrl.immediate_sound("PROMOTE");

	News* newsPtr = add_news( NEWS_UNIT_BETRAY, NEWS_NORMAL, unitPtr->nation_recno, betrayToNationRecno );

	if( !newsPtr )		// only news of nations that have contact with the player are added
		return;

	newsPtr->short_para1 = unitPtr->race_id;
	newsPtr->short_para2 = news_unit_name_id(unitPtr);
	newsPtr->short_para3 = unitPtr->rank_id;
	newsPtr->short_para4 = unitPtr->unit_id;

	//------- set location --------//

	if( betrayToNationRecno == nation_array.player_recno )
		newsPtr->set_loc( unitPtr->next_x_loc(), unitPtr->next_y_loc(), NEWS_LOC_UNIT, unitRecno, unitPtr->name_id );
	else
		newsPtr->set_loc( unitPtr->next_x_loc(), unitPtr->next_y_loc(), NEWS_LOC_ANY );
}
//------- End of function NewsArray::unit_betray -----//

//------ Begin of function NewsArray::unit_assassinated -----//
//
// <int> unitRecno - recno of the unit that has been assassinated.
// <int> spyKilled - whether the enemy spy has been killed during his assissination mission.
//
// ####### begin Gilbert 19/2 ########//
// short_para1 - rank id. of the assassinated unit
// short_para2 - race id. of the assassinated unit
// short_para3 - name id. of the assassinated unit
// it used to be para1 for race and para2 for name  and para3 for rank
// ####### end Gilbert 19/2 ########//
// short_para4 - whether the enemy spy has been killed or not.
//
void NewsArray::unit_assassinated(int unitRecno, int spyKilled)
{
	Unit* unitPtr = unit_array[unitRecno];

	err_when( unitPtr->nation_recno != nation_array.player_recno );

	News* newsPtr = add_news( NEWS_UNIT_ASSASSINATED, NEWS_NORMAL, unitPtr->nation_recno );

	if( !newsPtr )		// only news of nations that have contact with the player are added
		return;

	// ####### begin Gilbert 19/2 #########//
	newsPtr->short_para1 = unitPtr->rank_id;
	newsPtr->short_para2 = unitPtr->race_id;
	newsPtr->short_para3 = news_unit_name_id(unitPtr);
	// ####### end Gilbert 19/2 #########//
	newsPtr->short_para4 = spyKilled;

	//------- set location --------//

	short xLoc, yLoc;

	unitPtr->get_cur_loc(xLoc, yLoc);

	newsPtr->set_loc( xLoc, yLoc, NEWS_LOC_ANY );
}
//------- End of function NewsArray::unit_assassinated -----//


//------ Begin of function NewsArray::assassinator_caught -----//
//
// <int> spyRecno 	 - spy recno of the assassinator.
// <int> targetRankId - the rank id. of the assassinating target
// <int> targetRaceId - the race id. of the assassinating target
//
// short_para1 - rank id. of the assassinating target.
// short_para2 - race id. of the assassinating target.
//
void NewsArray::assassinator_caught(int spyRecno, int targetRankId, int targetRaceId )
{
	News* newsPtr = add_news( NEWS_ASSASSINATOR_CAUGHT, NEWS_NORMAL );

	if( !newsPtr )		// only news of nations that have contact with the player are added
		return;

	newsPtr->short_para1 = targetRankId;
	newsPtr->short_para2 = targetRaceId;

	//------- set location --------//

	int xLoc, yLoc;

	spy_array[spyRecno]->get_loc(xLoc, yLoc);

	newsPtr->set_loc( xLoc, yLoc, NEWS_LOC_ANY );
}
//------- End of function NewsArray::assassinator_caught -----//

//------ Begin of function NewsArray::general_die -----//
//
// <int> unitRecno - recno of the unit.
//
// short_para1 - race id. of your general
// short_para2 - name id. of your general
//
void NewsArray::general_die(int unitRecno)
{
	Unit* unitPtr = unit_array[unitRecno];

	err_when( unitPtr->nation_recno != nation_array.player_recno );

	//----- if last news is event_unit_die, don't generate a new news otherwise it will be duplicated ---//

	// ######## begin Gilbert 9/3 ######//
	if( size() > 0 )			// size may be zero
	{
		News* lastNews = operator[](size());		// news_array[size()];

		if( lastNews->id == NEWS_EV_UNIT_DIE &&
			lastNews->short_para1 == unitPtr->race_id &&
			lastNews->short_para2 == news_unit_name_id(unitPtr) ) // unitPtr->name_id )
		{
			return;
		}
	}
	// ######## end Gilbert 9/3 ######//

	//------ add the news now ------//

	News* newsPtr = add_news( NEWS_GENERAL_DIE, NEWS_NORMAL, unitPtr->nation_recno );

	if( !newsPtr )		// only news of nations that have contact with the player are added
		return;

	newsPtr->short_para1 = unitPtr->race_id;
	newsPtr->short_para2 = news_unit_name_id(unitPtr);

	//------- set location --------//

	short xLoc, yLoc;

	if( unitPtr->get_cur_loc(xLoc, yLoc) )
		newsPtr->set_loc( xLoc, yLoc, NEWS_LOC_ANY );
}
//------- End of function NewsArray::general_die -----//

//------ Begin of function NewsArray::general_form_new_nation -----//
//
// <int> unitRecno - recno of the unit.
//
// short_para1 - race id. of your general
// short_para2 - name id. of your general
//
void NewsArray::general_form_new_nation(int unitRecno)
{
	Unit* unitPtr = unit_array[unitRecno];

	News* newsPtr = add_news( NEWS_GENERAL_FORM_NEW_NATION, NEWS_NORMAL, unitPtr->nation_recno );

	if( !newsPtr )		// only news of nations that have contact with the player are added
		return;

	newsPtr->short_para1 = unitPtr->race_id;
	newsPtr->short_para2 = news_unit_name_id(unitPtr);

	//------- set location --------//

	short xLoc, yLoc;

	if( unitPtr->get_cur_loc(xLoc, yLoc) )
		newsPtr->set_loc( xLoc, yLoc, NEWS_LOC_ANY );
}
//------- End of function NewsArray::general_form_new_nation -----//


//------ Begin of function NewsArray::raw_exhaust -----//
//
// short_para1 - raw id.
// short_para2 - firm build id
//
void NewsArray::raw_exhaust(int rawId, int xLoc, int yLoc, int firmBuildId)
{
	News* newsPtr = add_news( NEWS_RAW_EXHAUST, NEWS_NORMAL );

	if( !newsPtr )		// only news of nations that have contact with the player are added
		return;

	newsPtr->short_para1 = rawId;
	newsPtr->short_para2 = firmBuildId;

	//------- set location --------//

	newsPtr->set_loc( xLoc, yLoc, NEWS_LOC_ANY );
}
//------- End of function NewsArray::raw_exhaust -----//


//------ Begin of function NewsArray::tech_researched -----//
//
// short_para1 - tech id.
// short_para2 - tech version.
//
void NewsArray::tech_researched(int techId, int techVersion)
{
	News* newsPtr = add_news( NEWS_TECH_RESEARCHED, NEWS_NORMAL, nation_array.player_recno );

	if( !newsPtr )		// only news of nations that have contact with the player are added
		return;

	newsPtr->short_para1 = techId;
	newsPtr->short_para2 = techVersion;

	//----- set location to any of towers of science of the player ------//

	for( int i=firm_array.size() ; i>0 ; i-- )
	{
		if( firm_array.is_deleted(i) )
			continue;

		Firm* firmPtr = firm_array[i];

		if( firmPtr->nation_recno == nation_array.player_recno &&
			 firmPtr->firm_id 	  == FIRM_RESEARCH &&
			 !firmPtr->under_construction )
		{
			newsPtr->set_loc( firmPtr->center_x, firmPtr->center_y, NEWS_LOC_FIRM, i );
			break;
		}
	}
}
//------- End of function NewsArray::tech_researched -----//


//------ Begin of function NewsArray::lightning_damage -----//
//
// short_para1 - object type NEWS_LOC_UNIT ...
// short_para2 - raceId, firm build id, 0
// short_para3 - nameId, near town name id, town name id
//
void NewsArray::lightning_damage(int xLoc, int yLoc, int objectId, int recno, int objectDie)
{
	News* newsPtr = add_news( NEWS_LIGHTNING_DAMAGE, NEWS_NORMAL );

	if( !newsPtr )
		return;

	newsPtr->set_loc( xLoc, yLoc, objectId, recno);

	newsPtr->short_para1 = objectId;
	newsPtr->short_para2 = 0;
	newsPtr->short_para3 = 0;
	newsPtr->short_para4 = 0;
	switch( objectId )
	{
	case NEWS_LOC_UNIT:
		newsPtr->short_para4 = unit_array[recno]->rank_id;
		newsPtr->short_para2 = unit_array[recno]->race_id;
		newsPtr->short_para3 = news_unit_name_id(unit_array[recno]);
		break;
	case NEWS_LOC_FIRM:
		newsPtr->short_para2 = firm_array[recno]->firm_build_id;
		if( firm_array[recno]->closest_town_name_id )
			newsPtr->short_para3 = firm_array[recno]->closest_town_name_id;
		else
			newsPtr->short_para3 = firm_array[recno]->get_closest_town_name_id();
		break;
	case NEWS_LOC_TOWN:
		newsPtr->short_para3 = town_array[recno]->town_name_id;
		break;
	default:
		err_here();
	}
	newsPtr->short_para5 = objectDie;
}
//------- End of function NewsArray::lightning_damage -----//


//------ Begin of function NewsArray::earthquake_damage -----//
//
void NewsArray::earthquake_damage(int unitDamage, int unitDie, int townDamage, int firmDamage, int firmDie)
{
	News* newsPtr;
	// ######## begin Gilbert 12/9 #######//
	if( unitDamage > 0 || unitDie > 0)
	{
		newsPtr = add_news( NEWS_EARTHQUAKE_DAMAGE, NEWS_NORMAL, nation_array.player_recno );
		if( newsPtr )
		{
			newsPtr->short_para1 = 1;
			newsPtr->short_para2 = unitDamage;
			newsPtr->short_para3 = unitDie;
		}
	}
	if( townDamage > 0)
	{
		newsPtr = add_news( NEWS_EARTHQUAKE_DAMAGE, NEWS_NORMAL, nation_array.player_recno );
		if( newsPtr )
		{
			newsPtr->short_para1 = 2;
			newsPtr->short_para2 = townDamage;
		}
	}
	if( firmDamage > 0 || firmDie > 0)
	{
		newsPtr = add_news( NEWS_EARTHQUAKE_DAMAGE, NEWS_NORMAL, nation_array.player_recno );
		if( newsPtr )
		{
			newsPtr->short_para1 = 3;
			newsPtr->short_para2 = firmDamage;
			newsPtr->short_para3 = firmDie;
		}
	}
	// ######## end Gilbert 12/9 #######//
}
//------- End of function NewsArray::earthquake_damage -----//


//------ Begin of function NewsArray::goal_deadline -----//
//
// Display a warning message as the deadline of the goals approaches.
//
// short_para1 - years left before the deadline.
// short_para2 - months left before the deadline.
//
void NewsArray::goal_deadline(int yearLeft, int monthLeft)
{
	News* newsPtr = add_news( NEWS_GOAL_DEADLINE, NEWS_NORMAL, nation_array.player_recno );

	if( !newsPtr )		// only news of nations that have contact with the player are added
		return;

	newsPtr->short_para1 = yearLeft;
	newsPtr->short_para2 = monthLeft;
}
//------- End of function NewsArray::goal_deadline -----//


//------ Begin of function NewsArray::weapon_ship_worn_out -----//
//
// Your weapon worn out and destroyed due to lack of money for
// maintenance.
//
// short_para1 - unit id. of the weapon
// short_para2 - level of the weapon
//
void NewsArray::weapon_ship_worn_out(int unitId, int weaponLevel)
{
	// ###### begin Gilbert 24/3 #########//
//	err_when( unit_res[unitId]->unit_class != UNIT_CLASS_WEAPON &&
//				 unit_res[unitId]->unit_class != UNIT_CLASS_SHIP && 
//				 unit_res[unitId]->unit_class != UNIT_CLASS_MONS_WEAPON );
	// ###### end Gilbert 24/3 #########//

	News* newsPtr = add_news( NEWS_WEAPON_SHIP_WORN_OUT, NEWS_NORMAL, nation_array.player_recno );

	if( !newsPtr )		// only news of nations that have contact with the player are added
		return;

	newsPtr->short_para1 = unitId;
	newsPtr->short_para2 = weaponLevel;
}
//------- End of function NewsArray::weapon_ship_worn_out -----//


//------ Begin of function NewsArray::firm_worn_out -----//
//
// <int> firmRecno - recno of the firm destroyed.
//
// short_para1 - firm build id. of the firm destroyed.
// short_para2 - name id of the town where the firm is located.
//
void NewsArray::firm_worn_out(int firmRecno)
{
	Firm* firmPtr = firm_array[firmRecno];

	err_when( firmPtr->nation_recno != nation_array.player_recno );

	News* newsPtr = add_news( NEWS_FIRM_WORN_OUT, NEWS_NORMAL, firmPtr->nation_recno);

	if( !newsPtr )		// only news of nations that have contact with the player are added
		return;

	newsPtr->short_para1 = firmPtr->firm_build_id;

	if( firmPtr->closest_town_name_id )
		newsPtr->short_para2 = firmPtr->closest_town_name_id;
	else
		newsPtr->short_para2 = firmPtr->get_closest_town_name_id();
}
//------- End of function NewsArray::firm_worn_out -----//


//------- Begin of function NewsArray::event_finish_tech -----//
//
// short_para1 - tech id
// short_para2 - tech version
//
void NewsArray::event_finish_tech(int techId, int techVersion)
{
	News* newsPtr = add_news( NEWS_EV_FINISH_TECH, NEWS_NORMAL, nation_array.player_recno);

	if( !newsPtr )		// only news of nations that have contact with the player are added
		return;

	newsPtr->short_para1 = techId;
	newsPtr->short_para2 = techVersion;
}
//------- End of function NewsArray::event_finish_tech -----//


//------- Begin of function NewsArray::event_new_tech -----//
//
// short_para1 - tech id
// short_para2 - tech version
//
void NewsArray::event_new_tech(int techId, int techVersion)
{
	News* newsPtr = add_news( NEWS_EV_NEW_TECH, NEWS_NORMAL, nation_array.player_recno);

	if( !newsPtr )		// only news of nations that have contact with the player are added
		return;

	newsPtr->short_para1 = techId;
	newsPtr->short_para2 = techVersion;
}
//------- End of function NewsArray::event_new_tech -----//


//------- Begin of function NewsArray::event_food_destroyed -----//
//
// short_para1 - food destroyed
//
void NewsArray::event_food_destroyed(int foodLost)
{
	News* newsPtr = add_news( NEWS_EV_FOOD_DESTROYED, NEWS_NORMAL, nation_array.player_recno);

	if( !newsPtr )		// only news of nations that have contact with the player are added
		return;

	newsPtr->short_para1 = foodLost;
}
//------- End of function NewsArray::event_food_destroyed -----//


//------- Begin of function NewsArray::event_unit_die -----//
//
// short_para1 - race of the person died
// short_para2 - name id of the person died
// short_para3 - rank of the person died
//
void NewsArray::event_unit_die(int unitRecno)
{
	if( unit_array.is_truly_deleted(unitRecno) )
		return;

	Unit *unitPtr = unit_array[unitRecno];

	err_when( !unitPtr->is_own() );

	News* newsPtr = add_news( NEWS_EV_UNIT_DIE, NEWS_NORMAL, nation_array.player_recno );

	if( !newsPtr )
		return;

//	err_when( !unitPtr->race_id );
	newsPtr->short_para1 = unitPtr->race_id;
	newsPtr->short_para2 = news_unit_name_id(unitPtr);
	newsPtr->short_para3 = unitPtr->rank_id;

	//------- set location --------//

	short xLoc, yLoc;

	if( unitPtr->get_cur_loc(xLoc, yLoc) )
		newsPtr->set_loc( xLoc, yLoc, NEWS_LOC_ANY );
}
//------- End of function NewsArray::event_unit_die -----//


//------- Begin of function NewsArray::event_soldier_die -----//
//
// short_para1 - firm build id
// short_para2 - town name id
// short_para3 - people died
// short_para4 - firm race id
//
void NewsArray::event_soldier_die(int firmRecno, int peopleKilled)
{
	Firm *firmPtr = firm_array[firmRecno];

	err_when( firmPtr->nation_recno != nation_array.player_recno );

	News* newsPtr = add_news( NEWS_EV_SOLDIER_DIE, NEWS_NORMAL, firmPtr->nation_recno );

	if( !newsPtr )
		return;

	newsPtr->short_para1 = firmPtr->firm_build_id;
	if( firmPtr->closest_town_name_id )
		newsPtr->short_para2 = firmPtr->closest_town_name_id;
	else
		newsPtr->short_para2 = firmPtr->get_closest_town_name_id();
	newsPtr->short_para3 = peopleKilled;
	// ####### begin Gilbert 12/2 #########//
	newsPtr->short_para4 = firmPtr->race_id;
	// ####### end Gilbert 12/2 #########//

	// ------- set location -----------//

	newsPtr->set_loc( firmPtr->center_x, firmPtr->center_y, NEWS_LOC_FIRM, firmRecno );
}
//------- End of function NewsArray::event_soldier_die -----//


//------- Begin of function NewsArray::event_item_found -----//
//
// short_para1 - town name id
//
void NewsArray::event_item_found(int townRecno)
{
	Town *townPtr = town_array[townRecno];

	err_when( townPtr->nation_recno != nation_array.player_recno );

	News* newsPtr = add_news( NEWS_EV_ITEM_FOUND, NEWS_NORMAL, townPtr->nation_recno );

	if( !newsPtr )
		return;

	newsPtr->short_para1 = townPtr->town_name_id;

	// ------- set location -----------//

	newsPtr->set_loc( townPtr->center_x, townPtr->center_y, NEWS_LOC_TOWN, townRecno );
}
//------- End of function NewsArray::event_item_found -----//


//------- Begin of function NewsArray::event_town_people_killed -----//
//
// short_para1 - town name id
// short_para2 - people killed
//
void NewsArray::event_town_people_killed(int townRecno, int peopleKilled)
{
	Town *townPtr = town_array[townRecno];

	err_when( townPtr->nation_recno != nation_array.player_recno );

	News* newsPtr = add_news( NEWS_EV_TOWN_PEOPLE_KILLED, NEWS_NORMAL, townPtr->nation_recno );

	if( !newsPtr )
		return;

	newsPtr->short_para1 = townPtr->town_name_id;
	newsPtr->short_para2 = peopleKilled;

	// ------- set location -----------//

	newsPtr->set_loc( townPtr->center_x, townPtr->center_y, NEWS_LOC_TOWN, townRecno );
}
//------- End of function NewsArray::event_town_people_killed -----//


//------- Begin of function NewsArray::event_factory_destroyed -----//
//
// short_para1 - firm build id
// short_para2 - nearest town name id
// short_para3 - people killed
//
void NewsArray::event_factory_destroyed(int firmRecno, int peopleKilled)
{
	Firm *firmPtr = firm_array[firmRecno];

	err_when( firmPtr->nation_recno != nation_array.player_recno );

	News* newsPtr = add_news( NEWS_EV_FACTORY_DESTROYED, NEWS_NORMAL, firmPtr->nation_recno );

	if( !newsPtr )
		return;

	newsPtr->short_para1 = firmPtr->firm_build_id;
	if( firmPtr->closest_town_name_id )
		newsPtr->short_para2 = firmPtr->closest_town_name_id;
	else
		newsPtr->short_para2 = firmPtr->get_closest_town_name_id();
	newsPtr->short_para3 = peopleKilled;

	// ------- set location -----------//

	newsPtr->set_loc( firmPtr->center_x, firmPtr->center_y, NEWS_LOC_ANY );
}
//------- End of function NewsArray::event_factory_destroyed -----//


//------- Begin of function NewsArray::event_mine_destroyed -----//
//
// short_para1 - firm build id
// short_para2 - nearest town name id
// short_para3 - people killed
//
void NewsArray::event_mine_destroyed(int firmRecno, int peopleKilled)
{
	Firm *firmPtr = firm_array[firmRecno];

	err_when( firmPtr->nation_recno != nation_array.player_recno );

	News* newsPtr = add_news( NEWS_EV_MINE_DESTROYED, NEWS_NORMAL, firmPtr->nation_recno );

	if( !newsPtr )
		return;

	newsPtr->short_para1 = firmPtr->firm_build_id;
	if( firmPtr->closest_town_name_id )
		newsPtr->short_para2 = firmPtr->closest_town_name_id;
	else
		newsPtr->short_para2 = firmPtr->get_closest_town_name_id();
	newsPtr->short_para3 = peopleKilled;

	// ------- set location -----------//

	newsPtr->set_loc( firmPtr->center_x, firmPtr->center_y, NEWS_LOC_ANY );
}
//------- End of function NewsArray::event_mine_destroyed -----//


//------- Begin of function NewsArray::event_inn_destroyed -----//
//
// short_para1 - firm build id
// short_para2 - town name id
//
void NewsArray::event_inn_destroyed(int firmRecno)
{
	Firm *firmPtr = firm_array[firmRecno];

	err_when( firmPtr->nation_recno != nation_array.player_recno );

	News* newsPtr = add_news( NEWS_EV_INN_DESTROYED, NEWS_NORMAL, firmPtr->nation_recno );

	if( !newsPtr )
		return;

	newsPtr->short_para1 = firmPtr->firm_build_id;
	if( firmPtr->closest_town_name_id )
		newsPtr->short_para2 = firmPtr->closest_town_name_id;
	else
		newsPtr->short_para2 = firmPtr->get_closest_town_name_id();

	// ------- set location -----------//

	newsPtr->set_loc( firmPtr->center_x, firmPtr->center_y, NEWS_LOC_ANY );
}
//------- End of function NewsArray::event_inn_destroyed -----//


//------- Begin of function NewsArray::tornado_hit -----//
//
// short_para1 - tornado recno
//
void NewsArray::tornado_hit(int tornadoRecno)
{
	Tornado *tornadoPtr = tornado_array[tornadoRecno];

	News *newsPtr = add_news (NEWS_TORNADO_HIT, NEWS_NORMAL );

	if( !newsPtr )
		return;

	newsPtr->short_para1 = tornadoRecno;

	newsPtr->set_loc( tornadoPtr->cur_x_loc(), tornadoPtr->cur_y_loc(), NEWS_LOC_TORNADO, tornadoRecno );
}
//------- End of function NewsArray::tornado_hit -----//


//------ Begin of function NewsArray::blueprint_acquired -----//
//
// <int> acquireNationRecno - recno of the nation that has acquired the scroll
// <int> techId             - race of the scroll
//
// nation_name1() - the nation that has acquired the scroll.
//
// short_para1 = the tech id. of the scroll.
//
void NewsArray::blueprint_acquired(int acquireNationRecno, int techId)
{
	News* newsPtr = add_news( NEWS_BLUEPRINT_ACQUIRED, NEWS_NORMAL, acquireNationRecno );

	if( !newsPtr )		// only news of nations that have contact with the player are added
		return;

	newsPtr->short_para1 = techId;
}
//------- End of function NewsArray::blueprint_acquired -----//


//------ Begin of function NewsArray::magic_tower_attack -----//
//
// <int> attackerBaseObjRecno - magic tower attacking
// <int> magicId            - magicId see of_magi.h
// <int> targetBaseObjRecno - target base obj recno , 0 then see xLoc, yLoc
// <int> xLoc, yLoc         - target location
//
// short_para1 - id. of the firm, 0 if town
// short_para2 - name id of the town where the firm is located.
// short_para3 - target's object type OBJ_UNIT/ OBJ_TOWN/ OBJ_FIRM
// short_para4 = the magic id
// loc_x, loc_y = xLoc, yLoc
//
void NewsArray::magic_tower_attack(int attackerBaseObjRecno, int magicId, int targetBaseObjRecno, int xLoc, int yLoc )
{
	BaseObj *targetObj, *attackerObj;
	if( targetBaseObjRecno && !base_obj_array.is_deleted(targetBaseObjRecno) )
		targetObj = base_obj_array[targetBaseObjRecno];
	else
	{
		targetObj = NULL;
		targetBaseObjRecno = 0;
	}

	if( attackerBaseObjRecno && !base_obj_array.is_deleted(attackerBaseObjRecno) )
		attackerObj = base_obj_array[attackerBaseObjRecno];
	else
	{
		attackerObj = NULL;
		attackerBaseObjRecno = 0;
	}

	News* newsPtr = add_news( NEWS_MAGIC_TOWER_ATTACK, NEWS_NORMAL, 
		(targetObj ? targetObj->nation_recno : 0),
		(attackerObj ? attackerObj->nation_recno : 0) );

	if( !newsPtr )		// only news of nations that have contact with the player are added
		return;

	newsPtr->short_para4 = magicId;

	// -------- set location --------//

	if( targetObj )
	{
		switch( targetObj->obj_type )
		{
		case OBJ_UNIT:
			{
				newsPtr->short_para1 = 0;		// to complicated to store unit name
				newsPtr->short_para2 = 0;
				newsPtr->short_para3 = targetObj->obj_type;
				newsPtr->set_loc(xLoc, yLoc, NEWS_LOC_UNIT, targetObj->obj_extern_recno, targetObj->cast_to_Unit()->name_id);
			}
			break;
		case OBJ_FIRM:
			{
				Firm *firmPtr = targetObj->cast_to_Firm();
				newsPtr->short_para1 = firmPtr->firm_build_id;
				if( firmPtr->closest_town_name_id )
					newsPtr->short_para2 = firmPtr->closest_town_name_id;
				else
					newsPtr->short_para2 = firmPtr->get_closest_town_name_id();
				newsPtr->short_para3 = targetObj->obj_type;
				newsPtr->set_loc(xLoc, yLoc, NEWS_LOC_FIRM, targetObj->obj_extern_recno);
			}
			break;
		case OBJ_TOWN:
			{
				Town *townPtr = targetObj->cast_to_Town();
				newsPtr->short_para1 = 0;
				newsPtr->short_para2 = townPtr->town_name_id;
				newsPtr->short_para3 = targetObj->obj_type;
				newsPtr->set_loc(xLoc, yLoc, NEWS_LOC_TOWN, targetObj->obj_extern_recno);
			}
			break;
		default:
			newsPtr->short_para3 = 0;
			newsPtr->set_loc(xLoc, yLoc, NEWS_LOC_ANY);
		}
	}
	else
	{
		newsPtr->short_para3 = 0;
		newsPtr->set_loc(xLoc, yLoc, NEWS_LOC_ANY);
	}
}
//------- End of function NewsArray::magic_tower_attack -----//


//------ Begin of function NewsArray::chat_msg -----//
//
// <int>   fromNationRecno - recno of the nation from which this chat message is sent.
// <char*> chatStr     	   - pointer to the chat string.
//
// short_para1 - id. of the chat msg in Info::remote_chat_str_array[]
//
// nation_name1() - the nation from which this chat message is sent.
//
void NewsArray::chat_msg(int fromNationRecno, char* chatStr)
{
	//---- add the chat string into Info::remote_chat_str_array[] ----//

	int useChatId=0;

	// ####### begin Gilbert 28/9 ######//
//	int minDate=info.game_date+1;
//	for( int i=0; i<MAX_REMOTE_CHAT_STR ; i++ )
//	{
//		if( info.remote_chat_array[i].received_date < minDate )	 // replace the oldest one
//		{
//			minDate	 = info.remote_chat_array[i].received_date;
//			useChatId = i+1;
//		}
//	}

	useChatId = info.append_chat( NULL );		// NULL: record to be filled in later
	// ####### end Gilbert 28/9 ######//

	if( useChatId )
	{
		ChatInfo* chatInfo = info.remote_chat_array+useChatId-1;

		chatInfo->received_date = info.game_date;
		chatInfo->from_nation_recno = fromNationRecno;

		strncpy( chatInfo->chat_str, chatStr, CHAT_STR_LEN );
		chatInfo->chat_str[CHAT_STR_LEN] = NULL;
	}

	//----------------------------------------------//

	News* newsPtr = add_news( NEWS_CHAT_MSG, NEWS_NORMAL, fromNationRecno);

	if( !newsPtr )		// only news of nations that have contact with the player are added
		return;

	newsPtr->short_para1 = useChatId;
}
//------- End of function NewsArray::chat_msg -----//


//------ Begin of function NewsArray::multi_retire -----//
//
// This function is called when a human player retires.
//
void NewsArray::multi_retire(int nationRecno)
{
	add_news( NEWS_MULTI_RETIRE, NEWS_NORMAL, nationRecno, nation_array.player_recno, 1 );		// add player recno as the 2nd parameter so this message is always displayed even if the player doesn't yet have contact with this nation
}
//------- End of function NewsArray::multi_retire -----//


//------ Begin of function NewsArray::multi_quit_game -----//
//
// This function is called when a human player quits the game.
//
void NewsArray::multi_quit_game(int nationRecno)
{
	add_news( NEWS_MULTI_QUIT_GAME, NEWS_NORMAL, nationRecno, nation_array.player_recno, 1 );		// add player recno as the 2nd parameter so this message is always displayed even if the player doesn't yet have contact with this nation
}
//------- End of function NewsArray::multi_quit_game -----//


//------ Begin of function NewsArray::multi_save_game -----//
//
// This function is called when a human player calls for saving the game.
//
void NewsArray::multi_save_game()
{
	add_news( NEWS_MULTI_SAVE_GAME, NEWS_NORMAL );
}
//------- End of function NewsArray::multi_save_game -----//


//------ Begin of function NewsArray::multi_connection_lost -----//
//
// This function is called when a human player's connection has been lost.
//
void NewsArray::multi_connection_lost(int nationRecno)
{
	add_news( NEWS_MULTI_CONNECTION_LOST, NEWS_NORMAL, nationRecno, nation_array.player_recno, 1 );		// add player recno as the 2nd parameter so this message is always displayed even if the player doesn't yet have contact with this nation
}
//------- End of function NewsArray::multi_connection_lost -----//


//------ Begin of function NewsArray::add_news -----//
//
// Called by news processing function to set news parameters
//
// <int> newsId       = the id. of the news
// <int> newsType     = news type
// [int] nationRecno  = nation recno of the news
// [int] nationRecno2 = recno of the 2nd nation related to the news
// [int] forceAdd		 = add this news anyway, regardless of whether
//								the nation has contact with the player or not
//                      (default: 0)
//
// return : <News*> return the pointer of the News
//						  NULL - the nation of the news does not have contact with the player
//
News* NewsArray::add_news(int newsId, int newsType, int nationRecno, int nationRecno2, int forceAdd)
{
	if( nation_array.player_recno==0 )		// if the player has lost
		return NULL;

	//----- only news of nations that have contact with the player are added ----//

	if( nation_array.player_recno && !forceAdd )
	{
		Nation* playerNation = ~nation_array;

		if( nationRecno && nationRecno != nation_array.player_recno )
		{
			if( !playerNation->get_relation(nationRecno)->has_contact )
				return NULL;
		}

		if( nationRecno2 && nationRecno2 != nation_array.player_recno )
		{
			if( !playerNation->get_relation(nationRecno2)->has_contact )
				return NULL;
		}
	}

	//----------------------------------------------//

	static News news;

	news.id   = newsId;
	news.type = newsType;
	news.news_date = info.game_date;
	news.loc_type = 0;

	Nation* nationPtr;

	if( nationRecno )
	{
		nationPtr = nation_array[nationRecno];

		news.nation_recno1 = nationPtr->nation_recno;
		news.nation_race_id1 = (char) nationPtr->race_id;
		news.nation_color1   = nationPtr->color_scheme_id;

		err_when( !news.nation_recno1 || !news.nation_race_id1 );
	}
	else
	{
		news.nation_recno1 = 0;
		news.nation_color1   = -1;
	}

	if( nationRecno2 )
	{
		nationPtr = nation_array[nationRecno2];

		news.nation_recno2 = nationPtr->nation_recno;
		news.nation_race_id2 = (char) nationPtr->race_id;
		news.nation_color2   = nationPtr->color_scheme_id;
	}
	else
	{
		news.nation_recno2 = 0;
		news.nation_color2   = -1;
	}

	//--- if the news adding flag is turned off, don't add the news ---//

	if( news_add_flag )
	{
		//--- if no. of news reaches max., delete the oldest one ---//

		if( size() >= MAX_NEWS )
		{
			start();
			linkout();

			if( last_clear_recno > 1 )
				last_clear_recno--;
		}

		//--------- link in a new news ---------//

		linkin(&news);

		return (News*) get();
	}
	else
	{
		return &news;
	}
}
//------- End of function NewsArray::add_news -----//


//------ Begin of function News::set_loc ------//
//
void News::set_loc(int xLoc, int yLoc, int locType, int locTypePara, int locTypePara2)
{
	loc_type 	   = locType;
	loc_type_para  = locTypePara;
	loc_type_para2 = locTypePara2;

	err_when( loc_type_para < 0 );
	err_when( loc_type_para2 < 0 );

	loc_x = xLoc;
	loc_y = yLoc;
}
//------- End of function News::set_loc -------//


//------ Begin of function News::is_loc_valid ------//
//
// Whether the location of this news is still valid.
//
int News::is_loc_valid()
{
	if( !loc_type )
		return 0;

	int rc=0;

	if( loc_type == NEWS_LOC_TOWN )
	{
		if( !town_array.is_deleted(loc_type_para) )
		{
			Town* townPtr = town_array[loc_type_para];

			rc = townPtr->center_x == loc_x &&
				  townPtr->center_y == loc_y;
		}
	}
	else if( loc_type == NEWS_LOC_FIRM )
	{
		if( !firm_array.is_deleted(loc_type_para) )
		{
			Firm* firmPtr = firm_array[loc_type_para];

			rc = firmPtr->center_x == loc_x &&
				  firmPtr->center_y == loc_y;
		}
	}
	else if( loc_type == NEWS_LOC_UNIT )
	{
		if( !unit_array.is_deleted(loc_type_para) )
		{
			Unit* unitPtr = unit_array[loc_type_para];

			if( unitPtr->name_id == loc_type_para2 )
			{
				//--- if the unit is no longer belong to our nation ----//
				//--- only keep track of the unit for one month --------//

				if( unitPtr->nation_recno == nation_array.player_recno ||
					 info.game_date < news_date + 30 )
				{
					if( unitPtr->get_cur_loc(loc_x, loc_y) )
					{
						Location* locPtr = world.get_loc(loc_x, loc_y);

						rc = locPtr->visit_level > 0;
					}
				}
			}
		}
	}
	else if( loc_type == NEWS_LOC_ANY )
	{
		rc = 1;
	}
	else if( loc_type == NEWS_LOC_TORNADO )
	{
		if( !tornado_array.is_deleted(loc_type_para) )
		{
			Tornado *tornadoPtr = tornado_array[loc_type_para];
			// leave 1 location within boundary
			if( tornadoPtr->cur_x_loc()+1 >= 0 
				&& tornadoPtr->cur_x_loc()-1 <= MAX_WORLD_X_LOC
				&& tornadoPtr->cur_y_loc()+1 >= 0
				&& tornadoPtr->cur_y_loc()-1 <= MAX_WORLD_Y_LOC )
			{
				loc_x = tornadoPtr->cur_x_loc();
				loc_y = tornadoPtr->cur_y_loc();
				rc = 1;
			}
		}
	}

	if( !rc )
		loc_type = 0;

	return rc;
}
//------- End of function News::is_loc_valid -------//


//------ Begin of function News::get_unit_name -----//
//
char* News::get_unit_name(int raceId, int nameId)
{
//	if( nameId > 0 )
//	{
//		if( raceId > 0 )
//			return race_res[raceId]->get_name(nameId);
//		else
//			return monster_res.get_name(nameId);
//	}
//	else if( nameId == 0 )
//	{
//		return unit_res[UNIT_WAGON]->name;	//"Wagon";
//	}
//	else  //--- if nameId is negative, it is a hero id. ---//
//	{
//		return hero_res[-nameId]->name;
//	}

	// ####### begin Gilbert 18/1 ####//
	if( raceId == 0 )		// nameId is unit Id
	{
		if( nameId )
			return unit_res[nameId]->name;
	}
	else if( raceId > 0 )
	{
		if( nameId > 0 )
			return race_res[raceId]->get_name(nameId);
		else if( nameId < 0 )
			return hero_res[-nameId]->name;
		else
			return unit_res[UNIT_WAGON]->name;
	}
	else if( raceId < 0 )
	{
		return monster_res.get_name(nameId);
	}

	return "";
	// ####### end Gilbert 18/1 ####//
}
//------- End of function News::get_unit_name -----//


//------ Begin of function News::get_unit_single_name -----//
//
char* News::get_unit_single_name(int raceId, int nameId)
{
//	if( nameId > 0 )
//	{
//		if( raceId > 0 )
//			return race_res[raceId]->get_single_name(nameId);
//		else
//			return monster_res.get_name(nameId);
//	}
//	else if( nameId == 0 )
//	{
//		return unit_res[UNIT_WAGON]->name;	//"Wagon";
//	}
//	else
//		return hero_res[-nameId]->name;

	// ####### begin Gilbert 18/1 ####//
	if( raceId == 0 )		// nameId is unit Id
	{
		if( nameId )
			return unit_res[nameId]->name;
	}
	else if( raceId > 0 )
	{
		if( nameId > 0 )
			return race_res[raceId]->get_single_name(nameId);
		else if( nameId < 0 )
			return hero_res[-nameId]->name;
		else
			return unit_res[UNIT_WAGON]->name;	// wagon
	}
	else if( raceId < 0 )
	{
		return monster_res.get_name(nameId);
	}

	return "";
	// ####### end Gilbert 18/1 ####//
}
//------- End of function News::get_unit_single_name -----//


//------ Begin of static function news_unit_name_id -----//
//
static int news_unit_name_id(Unit* unitPtr)
{
//	if( unitPtr->unit_id == UNIT_WAGON )
//		return 0;
//	else if( unitPtr->hero_id )
//		return -unitPtr->hero_id;
//	else
//		return unitPtr->name_id;
	// ###### begin Gilbert 12/1 #####//
	if( unitPtr->race_id == 0 )
		return unitPtr->unit_id;
	else if( unitPtr->unit_id == UNIT_WAGON )
	{
		err_when( !unitPtr->is_human() );
		return 0;
	}
	else if( unitPtr->hero_id )
		return -unitPtr->hero_id;
	else
		return unitPtr->name_id;
	// ###### end Gilbert 12/1 #####//
}
//------- End of static function news_unit_name_id -----//



//------ Begin of static function news_unit_name_id -----//
//
static int news_unit_name_id(Spy* spyPtr)
{
	return spyPtr->name_id;
}
//------- End of static function news_unit_name_id -----//

