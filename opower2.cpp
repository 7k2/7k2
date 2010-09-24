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

//Filename	  : OPOWER2.CPP
//Description : Object Power - part 2

#include <OPOWER.H>
#include <OMOUSE.H>
#include <OMOUSECR.H>
#include <OUNIT.H>
#include <OFIRM.H>
#include <OTOWN.H>
#include <OSITE.H>
#include <OSYS.H>
#include <OMODEID.H>
#include <ONATION.H>
#include <OBOX.H>
#include <OIMGRES.H>
#include <OMOUSECR.H>
#include <OWORLD.H>
#include <OINFO.H>
#include <OU_CARA.H>
#include <OSERES.H>
#include <OREMOTE.H>
#include <OWORLDMT.H>


//--------- Begin of function Power::get_link_icon ---------//
//
// <char> linkStatus - link status
// <int>  sameNation - whether the two firms are of the same nation
//							  (default: 0)
//
// return: <char*> the bitmap pointer of the link icon
//
char* Power::get_link_icon(char linkStatus, int sameNation)
{
	char  goodLinkName[9] = "LINK_EE1";
	// ###### begin Gilbert 18/11 #######//
	int phase = sys.frame_count % 15;
	goodLinkName[7] = char( '1'+ phase );
	if( goodLinkName[7] > '9' )
		goodLinkName[7] += 'A' - '9' - 1;
	// ###### end Gilbert 18/11 #######//

	switch( linkStatus )
	{
		case LINK_EE:
			return image_icon.get_ptr(goodLinkName);

		case LINK_ED:
			return image_icon.get_ptr("LINK_ED");

		case LINK_DE:
			return image_icon.get_ptr("LINK_DE");

		case LINK_DD:
			if( sameNation )
				return image_icon.get_ptr("LINK_DE");	// green cross for same nation firms
			else
				return image_icon.get_ptr("LINK_DD");	// red cross for different nation firms
	}

	err_here();

	return NULL;
}
//----------- End of function Power::get_link_icon -----------//


// ---------- Begin of function Power::choose_cursor --------//
//
int Power::choose_cursor(int scrnX, int scrnY,
		ScreenObjectType selectedObjectType, short selectedObjectRecno,
		ScreenObjectType pointingObjectType, short pointingObjectRecno)
{
	int selectedObjectId = 0;
	int pointingObjectId = 0;

	if(command_id)
	{
		if( scrnX >= ZOOM_X1 && scrnX <= ZOOM_X2 &&
			scrnY >= ZOOM_Y1 && scrnY <= ZOOM_Y2)
		{
			switch(command_id)
			{
			case COMMAND_BUILD_FIRM:
				return CURSOR_BUILD;
			case COMMAND_TRANSFORM_FORTRESS:
				return CURSOR_BUILD;
			case COMMAND_ASSIGN:
				return CURSOR_ASSIGN;
			case COMMAND_BURN:
				return CURSOR_BURN;
			case COMMAND_SETTLE:
				{
					char flagColor = 0;                     // CURSOR_SETTLE + 0 is a valid cursor
					Unit *activeUnit;
					if( nation_array.player_recno 
						&& !nation_array.is_deleted(nation_array.player_recno) )
						flagColor = (~nation_array)->color_scheme_id;
					if( command_unit_recno && !unit_array.is_deleted(command_unit_recno)
						&& (activeUnit = unit_array[command_unit_recno]) )
					{
						if( !activeUnit->nation_recno )
							flagColor = 0;                  // independent nation
						else if( !nation_array.is_deleted(activeUnit->nation_recno) )
							flagColor = nation_array[activeUnit->nation_recno]->color_scheme_id;
					}
					return CURSOR_SETTLE + flagColor;
				}
			case COMMAND_SET_CARAVAN_STOP:
				// ####### begin Gilbert 4/3 ######//
				if( pointingObjectType == SCREEN_OBJECT_FRIEND_FIRM
					|| pointingObjectType == SCREEN_OBJECT_ENEMY_FIRM
					|| pointingObjectType == SCREEN_OBJECT_ALLIED_FIRM )
				{
					pointingObjectId = firm_array[pointingObjectRecno]->firm_id;
					if( pointingObjectId == FIRM_MARKET
						|| pointingObjectId == FIRM_MINE
						|| pointingObjectId == FIRM_FACTORY )
					{
						if( !nation_array.is_deleted(firm_array[pointingObjectRecno]->nation_recno)
							&& ((UnitCaravan *)unit_array[selectedObjectRecno])->can_set_stop(pointingObjectRecno) )
							return CURSOR_SET_STOP;
						else
							return CURSOR_CANT_SET_STOP;
					}
				}
				return CURSOR_NORMAL;
				// ####### end Gilbert 4/3 ######//

			// ###### begin Gilbert 20/5 ########//
			//case COMMAND_GOD_CAST_POWER:
			//	return CURSOR_NORMAL;
			// ###### end Gilbert 20/5 ########//

			case COMMAND_FIRM_MAGIC:
			case COMMAND_MAGIC_TELEPORT_PRE:
			case COMMAND_MAGIC_TELEPORT_POST:
				return CURSOR_NORMAL;

			// ###### begin Gilbert 20/5 ########//
			case COMMAND_GOD_CAST_POWER:
			// ###### end Gilbert 20/5 ########//
			default:
				// ##### begin Gilbert 5/3 ########//
				switch( pointingObjectType)
				{
				case SCREEN_OBJECT_NONE:
				case SCREEN_OBJECT_WALL:
				case SCREEN_OBJECT_SITE:
					return CURSOR_NORMAL;
				case SCREEN_OBJECT_FRIEND_UNIT:
				case SCREEN_OBJECT_UNIT_GROUP:
				case SCREEN_OBJECT_FRIEND_TOWN:
				case SCREEN_OBJECT_FRIEND_FIRM:
					return CURSOR_NORMAL_C;
				case SCREEN_OBJECT_ENEMY_UNIT:
				case SCREEN_OBJECT_ENEMY_TOWN:
				case SCREEN_OBJECT_ENEMY_FIRM:
					return CURSOR_NORMAL_O;
				case SCREEN_OBJECT_ALLIED_UNIT:
				case SCREEN_OBJECT_ALLIED_TOWN:
				case SCREEN_OBJECT_ALLIED_FIRM:
					return CURSOR_NORMAL_A;
				default:
					err_here();
					return CURSOR_NORMAL;
				}
				// ##### end Gilbert 5/3 ########//
			}
		}
		else
		{
			return CURSOR_NORMAL;
		}
	}
	else
	{
		// power.command_id == 0
		// ------- check cursor inside zoom window -------//
		if( scrnX >= ZOOM_X1 && scrnX <= ZOOM_X2 &&
			scrnY >= ZOOM_Y1 && scrnY <= ZOOM_Y2)
		{
			// ------ inside zoom window, depend on selected and pointing object
			switch(selectedObjectType)
			{
			case SCREEN_OBJECT_NONE:
			case SCREEN_OBJECT_WALL:
			case SCREEN_OBJECT_SITE:
			case SCREEN_OBJECT_ENEMY_UNIT:
			case SCREEN_OBJECT_ALLIED_UNIT:
				{
					switch( pointingObjectType)
					{
					case SCREEN_OBJECT_NONE:
					case SCREEN_OBJECT_WALL:
					case SCREEN_OBJECT_SITE:
						return CURSOR_NORMAL;
					case SCREEN_OBJECT_FRIEND_UNIT:
					case SCREEN_OBJECT_UNIT_GROUP:
					case SCREEN_OBJECT_FRIEND_TOWN:
					case SCREEN_OBJECT_FRIEND_FIRM:
						return CURSOR_NORMAL_C;
					case SCREEN_OBJECT_ENEMY_UNIT:
					case SCREEN_OBJECT_ENEMY_TOWN:
					case SCREEN_OBJECT_ENEMY_FIRM:
						return CURSOR_NORMAL_O;
					case SCREEN_OBJECT_ALLIED_UNIT:
					case SCREEN_OBJECT_ALLIED_TOWN:
					case SCREEN_OBJECT_ALLIED_FIRM:
						return CURSOR_NORMAL_A;
					default:
						err_here();
						return CURSOR_NORMAL;
					}
				}
			case SCREEN_OBJECT_SPY_UNIT:
				{
					Unit *unitPtr = unit_array[selectedObjectRecno];
					short nationRecno = unitPtr->nation_recno;
					selectedObjectId = unitPtr->unit_id;

					switch( pointingObjectType )
					{
					case SCREEN_OBJECT_NONE:
					case SCREEN_OBJECT_WALL:
					case SCREEN_OBJECT_SITE:
						return CURSOR_NORMAL;

					case SCREEN_OBJECT_FRIEND_UNIT:
					case SCREEN_OBJECT_ENEMY_UNIT:
					case SCREEN_OBJECT_ALLIED_UNIT:
						{
							Unit *pUnit = unit_array[pointingObjectRecno];
							if( nationRecno == pUnit->nation_recno )
							{
								// same nation
								return choose_cursor_units(selectedObjectRecno, pointingObjectRecno);
							}
							else
							{
								return CURSOR_UNIT_O;
							}
						}
					case SCREEN_OBJECT_FRIEND_TOWN:
					case SCREEN_OBJECT_ENEMY_TOWN:
					case SCREEN_OBJECT_ALLIED_TOWN:
						{
							Town *pTown = town_array[pointingObjectRecno];

							if( pTown->can_accept_assign(selectedObjectRecno, nation_array.player_recno) )
								return CURSOR_ASSIGN;
							else
							{
								if( nationRecno == pTown->nation_recno )
									return CURSOR_UNIT_C;
								else
									return CURSOR_UNIT_O;
							}
						}

					case SCREEN_OBJECT_FRIEND_FIRM:
					case SCREEN_OBJECT_ENEMY_FIRM:
					case SCREEN_OBJECT_ALLIED_FIRM:
						{
							Firm *pFirm = firm_array[pointingObjectRecno];

							if( pFirm->can_accept_assign(selectedObjectRecno, nation_array.player_recno) )
								return CURSOR_ASSIGN;
							else
							{
								if( nationRecno == pFirm->nation_recno )
									return CURSOR_UNIT_C;
								else
									return CURSOR_UNIT_O;
							}
						}
					default:
						err_here();
						return CURSOR_NORMAL;
					}
				}
			case SCREEN_OBJECT_SPY_GROUP:
				{
					int arraySize = unit_array.size();
					int i;
					
					switch( pointingObjectType)
					{
					case SCREEN_OBJECT_NONE:
					case SCREEN_OBJECT_SITE:
						return CURSOR_UNIT;
					case SCREEN_OBJECT_FRIEND_UNIT:
					case SCREEN_OBJECT_ENEMY_UNIT:
					case SCREEN_OBJECT_ALLIED_UNIT:
						return choose_cursor_spy_group(pointingObjectRecno);
					case SCREEN_OBJECT_UNIT_GROUP:
					case SCREEN_OBJECT_SPY_GROUP:
						err_here();		// impossible to pointing at more than one unit
						return CURSOR_UNIT_C;
					case SCREEN_OBJECT_FRIEND_TOWN:
					case SCREEN_OBJECT_ENEMY_TOWN:
					case SCREEN_OBJECT_ALLIED_TOWN:
						{
							Town *pTown = town_array[pointingObjectRecno];
							for( i = 1; i < arraySize; ++i)
							{
								Unit *unitPtr;
								if( unit_array.is_deleted(i) || !(unitPtr = unit_array[i])->selected_flag )
									continue;
//								if( unitPtr->nation_recno == pTown->nation_recno
//									&& unitPtr->race_id == pTown->race_id
//									&& unitPtr->rank_id == RANK_SOLDIER 
//									&& unitPtr->skill_level()==0 )
								if( pTown->can_accept_assign(i, nation_array.player_recno) )
									return CURSOR_ASSIGN;
							}
						}
						return CURSOR_UNIT_C;

					case SCREEN_OBJECT_FRIEND_FIRM:
					case SCREEN_OBJECT_ENEMY_FIRM:
					case SCREEN_OBJECT_ALLIED_FIRM:
						{
							Firm *firmPtr = firm_array[pointingObjectRecno];

							for( i = 1; i < arraySize; ++i )
							{
								if( unit_array.is_deleted(i) )
									continue;

								if( unit_array[i]->selected_flag &&
									 firmPtr->can_accept_assign(i, nation_array.player_recno) )
								{
									return CURSOR_ASSIGN;
								}
							}
							return CURSOR_UNIT_C;
						}
						return CURSOR_UNIT_O;
					case SCREEN_OBJECT_WALL:
						return CURSOR_DESTRUCT;
					default:
						err_here();
						return CURSOR_UNIT;
					}
				}
				break;
			case SCREEN_OBJECT_FRIEND_UNIT:
				{
					Unit *unitPtr = unit_array[selectedObjectRecno];
					selectedObjectId = unitPtr->unit_id;
					char rankId = unitPtr->rank_id;

					switch( pointingObjectType)
					{
					case SCREEN_OBJECT_NONE:
					case SCREEN_OBJECT_SITE:
						return CURSOR_UNIT;
					case SCREEN_OBJECT_FRIEND_UNIT:
						return choose_cursor_units(selectedObjectRecno, pointingObjectRecno);
					case SCREEN_OBJECT_UNIT_GROUP:
					case SCREEN_OBJECT_SPY_GROUP:
						err_here();		// impossible to pointing at more than one unit
						return CURSOR_UNIT_C;
					case SCREEN_OBJECT_FRIEND_TOWN:
//							if( unitPtr->race_id == town_array[pointingObjectRecno]->race_id 
//								&& rankId == RANK_SOLDIER && unitPtr->skill_level()==0 )
							if( town_array[pointingObjectRecno]->can_accept_assign( selectedObjectRecno, nation_array.player_recno) )
							return CURSOR_ASSIGN;
						else
							return CURSOR_UNIT_C;

					case SCREEN_OBJECT_FRIEND_FIRM:
						if( firm_array[pointingObjectRecno]->can_accept_assign(selectedObjectRecno, nation_array.player_recno) )
							return CURSOR_ASSIGN;
						else
							return CURSOR_UNIT_C;

					case SCREEN_OBJECT_ENEMY_UNIT:
					case SCREEN_OBJECT_ENEMY_TOWN:
					case SCREEN_OBJECT_ENEMY_FIRM:
						return CURSOR_UNIT_O;
					case SCREEN_OBJECT_ALLIED_UNIT:
					case SCREEN_OBJECT_ALLIED_TOWN:
					case SCREEN_OBJECT_ALLIED_FIRM:
						return CURSOR_UNIT_A;
					case SCREEN_OBJECT_WALL:
						return CURSOR_DESTRUCT;
					default:
						err_here();
						return CURSOR_UNIT;
					}
				}
			case SCREEN_OBJECT_UNIT_GROUP:
				{
					int arraySize = unit_array.size();
					int i;

					switch( pointingObjectType)
					{
					case SCREEN_OBJECT_NONE:
					case SCREEN_OBJECT_SITE:
						return CURSOR_UNIT;
					case SCREEN_OBJECT_FRIEND_UNIT:
						return choose_cursor_unit_group(pointingObjectRecno);
					case SCREEN_OBJECT_UNIT_GROUP:
					case SCREEN_OBJECT_SPY_GROUP:
						err_here();		// impossible to pointing at more than one unit
						return CURSOR_UNIT_C;
					case SCREEN_OBJECT_FRIEND_TOWN:
						{
							Town *pTown = town_array[pointingObjectRecno];
							for( i = 1; i < arraySize; ++i)
							{
								Unit *unitPtr;
								if( unit_array.is_deleted(i) || !(unitPtr = unit_array[i])->selected_flag )
									continue;
//								if( unitPtr->race_id == town_array[pointingObjectRecno]->race_id
//									&& unitPtr->rank_id == RANK_SOLDIER 
//									&& unitPtr->skill_level()==0 )
								if( pTown->can_accept_assign(i, nation_array.player_recno) )
									return CURSOR_ASSIGN;
							}
						}
						return CURSOR_UNIT_C;

					case SCREEN_OBJECT_FRIEND_FIRM:
						{
							Firm *firmPtr = firm_array[pointingObjectRecno];

							for( i = 1; i < arraySize; ++i )
							{
								if( unit_array.is_deleted(i) )
									continue;

								if( unit_array[i]->selected_flag &&
									 firmPtr->can_accept_assign(i, nation_array.player_recno) )
								{
									return CURSOR_ASSIGN;
								}
							}

							return CURSOR_UNIT_C;
						}
					case SCREEN_OBJECT_ENEMY_UNIT:
					case SCREEN_OBJECT_ENEMY_TOWN:
					case SCREEN_OBJECT_ENEMY_FIRM:
						return CURSOR_UNIT_O;
					case SCREEN_OBJECT_ALLIED_UNIT:
					case SCREEN_OBJECT_ALLIED_TOWN:
					case SCREEN_OBJECT_ALLIED_FIRM:
						return CURSOR_UNIT_A;
					case SCREEN_OBJECT_WALL:
						return CURSOR_DESTRUCT;
					default:
						err_here();
						return CURSOR_UNIT;
					}
				}
			case SCREEN_OBJECT_FRIEND_TOWN:
				{
					Town *townPtr = town_array[selectedObjectRecno];

					switch( pointingObjectType)
					{
					case SCREEN_OBJECT_NONE:
					case SCREEN_OBJECT_WALL:
					case SCREEN_OBJECT_SITE:
						return CURSOR_UNIT;
					case SCREEN_OBJECT_FRIEND_UNIT:
					case SCREEN_OBJECT_UNIT_GROUP:
						return CURSOR_UNIT_C;
					case SCREEN_OBJECT_FRIEND_FIRM:
						{
							Firm *pFirm = firm_array[pointingObjectRecno];
							int centerX = world.zoom_matrix->calc_zoom_x( 
								(pFirm->loc_x1+pFirm->loc_x2+1)*LOCATE_WIDTH/2,
								(pFirm->loc_y1+pFirm->loc_y2+1)*LOCATE_HEIGHT/2,
								pFirm->altitude);
							int centerY = world.zoom_matrix->calc_zoom_y( 
								(pFirm->loc_x1+pFirm->loc_x2+1)*LOCATE_WIDTH/2,
								(pFirm->loc_y1+pFirm->loc_y2+1)*LOCATE_HEIGHT/2,
								pFirm->altitude);
							if(m.points_distance( mouse.cur_x, mouse.cur_y, centerX+ZOOM_X1, centerY+ZOOM_Y1) <= 11
								&& townPtr->can_toggle_firm_link(pointingObjectRecno) )
							{
								return CURSOR_ON_LINK;
							}
						}
						return CURSOR_UNIT_C;
					case SCREEN_OBJECT_FRIEND_TOWN:
						if( selectedObjectRecno != pointingObjectRecno )
						{
							// check if the cursor is pointing at the middle of the town
							Town *pTown = town_array[pointingObjectRecno];
							int centerX = world.zoom_matrix->calc_zoom_x( 
								(pTown->loc_x1+pTown->loc_x2+1)*LOCATE_WIDTH/2,
								(pTown->loc_y1+pTown->loc_y2+1)*LOCATE_HEIGHT/2,
								pTown->altitude);
							int centerY = world.zoom_matrix->calc_zoom_y( 
								(pTown->loc_x1+pTown->loc_x2+1)*LOCATE_WIDTH/2,
								(pTown->loc_y1+pTown->loc_y2+1)*LOCATE_HEIGHT/2,
								pTown->altitude);
							if(m.points_distance( mouse.cur_x, mouse.cur_y, centerX+ZOOM_X1, centerY+ZOOM_Y1) <= 11
								&& townPtr->can_migrate(pointingObjectRecno) )
							{
								return CURSOR_ON_LINK;
							}
						}
						return CURSOR_UNIT_C;
					case SCREEN_OBJECT_ENEMY_UNIT:
					case SCREEN_OBJECT_ENEMY_TOWN:
						return CURSOR_UNIT_O;
					case SCREEN_OBJECT_ENEMY_FIRM:
						{
							Firm *pFirm = firm_array[pointingObjectRecno];
							int centerX = world.zoom_matrix->calc_zoom_x( 
								(pFirm->loc_x1+pFirm->loc_x2+1)*LOCATE_WIDTH/2,
								(pFirm->loc_y1+pFirm->loc_y2+1)*LOCATE_HEIGHT/2,
								pFirm->altitude);
							int centerY = world.zoom_matrix->calc_zoom_y( 
								(pFirm->loc_x1+pFirm->loc_x2+1)*LOCATE_WIDTH/2,
								(pFirm->loc_y1+pFirm->loc_y2+1)*LOCATE_HEIGHT/2,
								pFirm->altitude);
							if(m.points_distance( mouse.cur_x, mouse.cur_y, centerX+ZOOM_X1, centerY+ZOOM_Y1) <= 11
								&& townPtr->can_toggle_firm_link(pointingObjectRecno) )
							{
								return CURSOR_ON_LINK;
							}
						}
						return CURSOR_UNIT_O;
					case SCREEN_OBJECT_ALLIED_UNIT:
					case SCREEN_OBJECT_ALLIED_TOWN:
						return CURSOR_UNIT_A;
					case SCREEN_OBJECT_ALLIED_FIRM:
						{
							Firm *pFirm = firm_array[pointingObjectRecno];
							int centerX = world.zoom_matrix->calc_zoom_x( 
								(pFirm->loc_x1+pFirm->loc_x2+1)*LOCATE_WIDTH/2,
								(pFirm->loc_y1+pFirm->loc_y2+1)*LOCATE_HEIGHT/2,
								pFirm->altitude);
							int centerY = world.zoom_matrix->calc_zoom_y( 
								(pFirm->loc_x1+pFirm->loc_x2+1)*LOCATE_WIDTH/2,
								(pFirm->loc_y1+pFirm->loc_y2+1)*LOCATE_HEIGHT/2,
								pFirm->altitude);
							if(m.points_distance( mouse.cur_x, mouse.cur_y, centerX+ZOOM_X1, centerY+ZOOM_Y1) <= 11
								&& townPtr->can_toggle_firm_link(pointingObjectRecno) )
							{
								return CURSOR_ON_LINK;
							}
						}
						return CURSOR_UNIT_A;
					default:
						err_here();
						return CURSOR_UNIT;
					}
				}
			case SCREEN_OBJECT_ENEMY_TOWN:
			case SCREEN_OBJECT_ALLIED_TOWN:
				{
					Town *townPtr = town_array[selectedObjectRecno];
					switch( pointingObjectType)
					{
					case SCREEN_OBJECT_NONE:
					case SCREEN_OBJECT_WALL:
					case SCREEN_OBJECT_SITE:
						return CURSOR_UNIT;
					case SCREEN_OBJECT_FRIEND_UNIT:
					case SCREEN_OBJECT_UNIT_GROUP:
					case SCREEN_OBJECT_FRIEND_TOWN:
						return CURSOR_UNIT_C;
					case SCREEN_OBJECT_FRIEND_FIRM:
						return CURSOR_UNIT_C;
					case SCREEN_OBJECT_ENEMY_UNIT:
					case SCREEN_OBJECT_ENEMY_TOWN:
					case SCREEN_OBJECT_ENEMY_FIRM:
						return CURSOR_UNIT_O;
					case SCREEN_OBJECT_ALLIED_UNIT:
					case SCREEN_OBJECT_ALLIED_TOWN:
					case SCREEN_OBJECT_ALLIED_FIRM:
						return CURSOR_UNIT_A;
					default:
						err_here();
						return CURSOR_UNIT;
					}
				}
			case SCREEN_OBJECT_FRIEND_FIRM:
				{
					Firm *firmPtr = firm_array[selectedObjectRecno];
					switch( pointingObjectType)
					{
					case SCREEN_OBJECT_NONE:
					case SCREEN_OBJECT_WALL:
					case SCREEN_OBJECT_SITE:
						return CURSOR_UNIT;
					case SCREEN_OBJECT_FRIEND_UNIT:
					case SCREEN_OBJECT_UNIT_GROUP:
						return CURSOR_UNIT_C;
					case SCREEN_OBJECT_FRIEND_TOWN:
						{
							Town *pTown = town_array[pointingObjectRecno];
							int centerX = world.zoom_matrix->calc_zoom_x( 
								(pTown->loc_x1+pTown->loc_x2+1)*LOCATE_WIDTH/2,
								(pTown->loc_y1+pTown->loc_y2+1)*LOCATE_HEIGHT/2,
								pTown->altitude);
							int centerY = world.zoom_matrix->calc_zoom_y( 
								(pTown->loc_x1+pTown->loc_x2+1)*LOCATE_WIDTH/2,
								(pTown->loc_y1+pTown->loc_y2+1)*LOCATE_HEIGHT/2,
								pTown->altitude);
							if(m.points_distance( mouse.cur_x, mouse.cur_y, centerX+ZOOM_X1, centerY+ZOOM_Y1) <= 11
								&& firmPtr->can_toggle_town_link(pointingObjectRecno) )
							{
								return CURSOR_ON_LINK;
							}
						}
						return CURSOR_UNIT_C;
					case SCREEN_OBJECT_FRIEND_FIRM:
						if( selectedObjectRecno != pointingObjectRecno )
						{
							Firm *pFirm = firm_array[pointingObjectRecno];
							int centerX = world.zoom_matrix->calc_zoom_x( 
								(pFirm->loc_x1+pFirm->loc_x2+1)*LOCATE_WIDTH/2,
								(pFirm->loc_y1+pFirm->loc_y2+1)*LOCATE_HEIGHT/2,
								pFirm->altitude);
							int centerY = world.zoom_matrix->calc_zoom_y( 
								(pFirm->loc_x1+pFirm->loc_x2+1)*LOCATE_WIDTH/2,
								(pFirm->loc_y1+pFirm->loc_y2+1)*LOCATE_HEIGHT/2,
								pFirm->altitude);
							if(m.points_distance( mouse.cur_x, mouse.cur_y, centerX+ZOOM_X1, centerY+ZOOM_Y1) <= 11
								&& firmPtr->can_toggle_firm_link(pointingObjectRecno) )
							{
								return CURSOR_ON_LINK;
							}
						}
						return CURSOR_UNIT_C;
					case SCREEN_OBJECT_ENEMY_UNIT:
						return CURSOR_UNIT_O;
					case SCREEN_OBJECT_ENEMY_TOWN:
						{
							Town *pTown = town_array[pointingObjectRecno];
							int centerX = world.zoom_matrix->calc_zoom_x( 
								(pTown->loc_x1+pTown->loc_x2+1)*LOCATE_WIDTH/2,
								(pTown->loc_y1+pTown->loc_y2+1)*LOCATE_HEIGHT/2,
								pTown->altitude);
							int centerY = world.zoom_matrix->calc_zoom_y( 
								(pTown->loc_x1+pTown->loc_x2+1)*LOCATE_WIDTH/2,
								(pTown->loc_y1+pTown->loc_y2+1)*LOCATE_HEIGHT/2,
								pTown->altitude);
							if(m.points_distance( mouse.cur_x, mouse.cur_y, centerX+ZOOM_X1, centerY+ZOOM_Y1) <= 11
								&& firmPtr->can_toggle_town_link(pointingObjectRecno) )
							{
								return CURSOR_ON_LINK;
							}
						}
						return CURSOR_UNIT_O;
					case SCREEN_OBJECT_ENEMY_FIRM:
						{
							Firm *pFirm = firm_array[pointingObjectRecno];
							int centerX = world.zoom_matrix->calc_zoom_x( 
								(pFirm->loc_x1+pFirm->loc_x2+1)*LOCATE_WIDTH/2,
								(pFirm->loc_y1+pFirm->loc_y2+1)*LOCATE_HEIGHT/2,
								pFirm->altitude);
							int centerY = world.zoom_matrix->calc_zoom_y( 
								(pFirm->loc_x1+pFirm->loc_x2+1)*LOCATE_WIDTH/2,
								(pFirm->loc_y1+pFirm->loc_y2+1)*LOCATE_HEIGHT/2,
								pFirm->altitude);
							if(m.points_distance( mouse.cur_x, mouse.cur_y, centerX+ZOOM_X1, centerY+ZOOM_Y1) <= 11
								&& firmPtr->can_toggle_firm_link(pointingObjectRecno) )
							{
								return CURSOR_ON_LINK;
							}
						}
						return CURSOR_UNIT_O;
					case SCREEN_OBJECT_ALLIED_UNIT:
						return CURSOR_UNIT_A;
					case SCREEN_OBJECT_ALLIED_TOWN:
						{
							Town *pTown = town_array[pointingObjectRecno];
							int centerX = world.zoom_matrix->calc_zoom_x( 
								(pTown->loc_x1+pTown->loc_x2+1)*LOCATE_WIDTH/2,
								(pTown->loc_y1+pTown->loc_y2+1)*LOCATE_HEIGHT/2,
								pTown->altitude);
							int centerY = world.zoom_matrix->calc_zoom_y( 
								(pTown->loc_x1+pTown->loc_x2+1)*LOCATE_WIDTH/2,
								(pTown->loc_y1+pTown->loc_y2+1)*LOCATE_HEIGHT/2,
								pTown->altitude);
							if(m.points_distance( mouse.cur_x, mouse.cur_y, centerX+ZOOM_X1, centerY+ZOOM_Y1) <= 11
								&& firmPtr->can_toggle_town_link(pointingObjectRecno) )
							{
								return CURSOR_ON_LINK;
							}
						}
						return CURSOR_UNIT_A;
					case SCREEN_OBJECT_ALLIED_FIRM:
						{
							Firm *pFirm = firm_array[pointingObjectRecno];
							int centerX = world.zoom_matrix->calc_zoom_x( 
								(pFirm->loc_x1+pFirm->loc_x2+1)*LOCATE_WIDTH/2,
								(pFirm->loc_y1+pFirm->loc_y2+1)*LOCATE_HEIGHT/2,
								pFirm->altitude);
							int centerY = world.zoom_matrix->calc_zoom_y( 
								(pFirm->loc_x1+pFirm->loc_x2+1)*LOCATE_WIDTH/2,
								(pFirm->loc_y1+pFirm->loc_y2+1)*LOCATE_HEIGHT/2,
								pFirm->altitude);
							if(m.points_distance( mouse.cur_x, mouse.cur_y, centerX+ZOOM_X1, centerY+ZOOM_Y1) <= 11
								&& firmPtr->can_toggle_firm_link(pointingObjectRecno) )
							{
								return CURSOR_ON_LINK;
							}
						}
						return CURSOR_UNIT_A;
					default:
						err_here();
						return CURSOR_UNIT;
					}
				}
			case SCREEN_OBJECT_ENEMY_FIRM:
			case SCREEN_OBJECT_ALLIED_FIRM:
				{
					switch( pointingObjectType)
					{
					case SCREEN_OBJECT_NONE:
					case SCREEN_OBJECT_WALL:
					case SCREEN_OBJECT_SITE:
						return CURSOR_UNIT;
					case SCREEN_OBJECT_FRIEND_UNIT:
					case SCREEN_OBJECT_UNIT_GROUP:
					case SCREEN_OBJECT_FRIEND_FIRM:
						return CURSOR_UNIT_C;
					case SCREEN_OBJECT_FRIEND_TOWN:
						return CURSOR_UNIT_C;
					case SCREEN_OBJECT_ENEMY_UNIT:
					case SCREEN_OBJECT_ENEMY_TOWN:
					case SCREEN_OBJECT_ENEMY_FIRM:
						return CURSOR_UNIT_O;
					case SCREEN_OBJECT_ALLIED_UNIT:
					case SCREEN_OBJECT_ALLIED_TOWN:
					case SCREEN_OBJECT_ALLIED_FIRM:
						return CURSOR_UNIT_A;
					default:
						err_here();
						return CURSOR_UNIT;
					}
				}
			default:
				err_here();
				return CURSOR_NORMAL;
			}
		}
		else
		{
			// ------ outside zoom area depend on selected object ------//
			switch(selectedObjectType)
			{
			case SCREEN_OBJECT_NONE:
			case SCREEN_OBJECT_WALL:
			case SCREEN_OBJECT_SITE:
			case SCREEN_OBJECT_ENEMY_UNIT:
			case SCREEN_OBJECT_ALLIED_UNIT:
				return CURSOR_NORMAL;

			case SCREEN_OBJECT_FRIEND_UNIT:
			case SCREEN_OBJECT_UNIT_GROUP:
			case SCREEN_OBJECT_SPY_UNIT:
			case SCREEN_OBJECT_SPY_GROUP:
				return CURSOR_UNIT;

			case SCREEN_OBJECT_FRIEND_TOWN:
				return CURSOR_UNIT;

			case SCREEN_OBJECT_ENEMY_TOWN:
			case SCREEN_OBJECT_ALLIED_TOWN:
				return CURSOR_UNIT;

			case SCREEN_OBJECT_FRIEND_FIRM:
				return CURSOR_UNIT;

			case SCREEN_OBJECT_ENEMY_FIRM:
			case SCREEN_OBJECT_ALLIED_FIRM:
				return CURSOR_UNIT;

			default:
				err_here();
				return CURSOR_NORMAL;
			}
		}
	}
}
// ---------- End of function Power::choose_cursor --------//


// ---------- Begin of function Power::choose_cursor_units --------//
//
int Power::choose_cursor_units(short selectedUnitRecno, short pointingUnitRecno)
{
	Unit *u1Ptr = unit_array[selectedUnitRecno];
	Unit *u2Ptr = unit_array[pointingUnitRecno];
	int selectedUnitId = u1Ptr->unit_id;
	int pointingUnitId = u2Ptr->unit_id;
	UnitInfo *u1 = unit_res[selectedUnitId];
	UnitInfo *u2 = unit_res[pointingUnitId];
	if(u1 && u2)
	{
		// ------- detect ship that can carry land unit -------//
//		if( u1->mobile_type == UNIT_LAND &&
//			u2 && u2->carry_unit_capacity > 0)
		if( u2Ptr->can_accept_assign(selectedUnitRecno) )
		{
			return CURSOR_ASSIGN;
		}
		else if( pointingUnitId == UNIT_EXPLOSIVE_CART &&		// check trigger explosive cart
			u1Ptr->max_attack_range() > 1)
		{
			return CURSOR_TRIGGER_EXPLODE;
		}
//		else if( u1->vehicle_id == pointingUnitId &&		// can ride on
//			u1->solider_id != 0)		// make sure u1 is a rider, not a riding unit
//		{
//			return CURSOR_ASSIGN;
//		}
	}

	return CURSOR_UNIT_C;
}
// ---------- End of function Power::choose_cursor_units --------//


// ---------- Begin of function Power::choose_cursor_unit_group --------//
//
int Power::choose_cursor_unit_group(short pointingUnitRecno)
{
	Unit *u2Ptr = unit_array[pointingUnitRecno];
	int pointingUnitId = u2Ptr->unit_id;

	// ----- assume all selected unit are owned by the player ----//

	UnitInfo *u2 = unit_res[pointingUnitId];
	err_when( !u2);

	int s = unit_array.size();
	for( int i=1; i <= s; ++i )
	{
		if( !unit_array.is_deleted(i) && unit_array[i]->selected_flag)
		{
			if( u2Ptr->can_accept_assign(i, nation_array.player_recno ) )
				return CURSOR_ASSIGN;
		}
	}

	if( pointingUnitId == UNIT_EXPLOSIVE_CART )
	{
		for( i=1; i <= s; ++i )
		{
			Unit *unitPtr;
			if( !unit_array.is_deleted(i) && (unitPtr=unit_array[i])->selected_flag)
			{
				if( unitPtr->max_attack_range() > 1)
					return CURSOR_TRIGGER_EXPLODE;
			}
		}
	}

	return CURSOR_UNIT_C;
}
// ---------- End of function Power::choose_cursor_unit_group --------//


// ---------- begin of function Power::choose_cursor_spy_group ----------//
//
int Power::choose_cursor_spy_group(short pointingUnitRecno)
{
	Unit *u2Ptr = unit_array[pointingUnitRecno];
	int pointingUnitId = u2Ptr->unit_id;

	// ----- assume all selected unit are owned by the player ----//

	UnitInfo *u2 = unit_res[pointingUnitId];
	err_when( !u2);

	int s = unit_array.size();
	for( int i=1; i <= s; ++i )
	{
		if( !unit_array.is_deleted(i) && unit_array[i]->selected_flag)
		{
			if( u2Ptr->can_accept_assign(i, nation_array.player_recno ) )
				return CURSOR_ASSIGN;
		}
	}

	return CURSOR_UNIT_C;
}
// ---------- end of function Power::choose_cursor_spy_group ----------//


// ---------- Begin of function Power::find_selected_type --------//
//
ScreenObjectType Power::find_selected_type( short *selectedRecno)
{
	short dummyId;
	if( selectedRecno == NULL)
		selectedRecno = &dummyId;

	if( unit_array.selected_recno )
	{
		// -------- check selected single unit ---------//

		if( unit_array.selected_count == 1)
		{
			Unit *unitPtr = unit_array[*selectedRecno = unit_array.selected_recno];
			if( unitPtr->nation_recno == nation_array.player_recno )
				return SCREEN_OBJECT_FRIEND_UNIT;
			else if( unitPtr->is_own_spy() )
				return SCREEN_OBJECT_SPY_UNIT;
			else if( nation_array.player_recno && unitPtr->nation_recno 
				&& (~nation_array)->get_relation_status(unitPtr->nation_recno) >= RELATION_FRIENDLY )
				return SCREEN_OBJECT_ALLIED_UNIT;
			else
				return SCREEN_OBJECT_ENEMY_UNIT;
		}

		// -------- check selected a group of units ---------//

		if( unit_array.selected_count > 1)
		{
			Unit *unitPtr = unit_array[(*selectedRecno = unit_array.selected_recno)];
			if( unitPtr->nation_recno == nation_array.player_recno )
				return SCREEN_OBJECT_UNIT_GROUP;
			else if( unitPtr->is_own_spy() )
				return SCREEN_OBJECT_SPY_GROUP;
			else if( nation_array.player_recno && unitPtr->nation_recno 
				&& (~nation_array)->get_relation_status(unitPtr->nation_recno) >= RELATION_FRIENDLY )
				return SCREEN_OBJECT_ALLIED_UNIT;
			else
			{
				// cannot select multiple enemy unit
				return SCREEN_OBJECT_ENEMY_UNIT;
			}
		}
	}

	// -------- check selected a firm ---------//

	if( firm_array.selected_recno )
	{
		Firm *firmPtr = firm_array[*selectedRecno = firm_array.selected_recno];
		if( firmPtr->is_own() )
			return SCREEN_OBJECT_FRIEND_FIRM;
		else if( nation_array.player_recno && firmPtr->nation_recno 
			&& (~nation_array)->get_relation_status(firmPtr->nation_recno) >= RELATION_FRIENDLY )
			return SCREEN_OBJECT_ALLIED_FIRM;
		else
			return SCREEN_OBJECT_ENEMY_FIRM;
	}

	// -------- check selected a town ---------//

	if( town_array.selected_recno )
	{
		Town *townPtr = town_array[*selectedRecno = town_array.selected_recno];
		if( townPtr->is_own() )
			return SCREEN_OBJECT_FRIEND_TOWN;
		else if( nation_array.player_recno && townPtr->nation_recno 
			&& (~nation_array)->get_relation_status(townPtr->nation_recno) >= RELATION_FRIENDLY )
			return SCREEN_OBJECT_ALLIED_TOWN;
		else
			return SCREEN_OBJECT_ENEMY_TOWN;
	}

	// -------- check selected a site ---------//

	if( site_array.selected_recno )
	{
		Site *sitePtr = site_array[*selectedRecno = site_array.selected_recno];
		return SCREEN_OBJECT_SITE;
	}

	return SCREEN_OBJECT_NONE;
}
// ---------- End of function Power::find_selected_type --------//


// ---------- Begin of function Power::find_pointing_type --------//
//
ScreenObjectType Power::find_pointing_type( Location *locPtr, short *pointingRecno)
{
	short dummyId;
	if( pointingRecno == NULL)
		pointingRecno = &dummyId;

	// ------- check pointing at unit, always check air unit first -------//

	if( locPtr->unit_recno( UNIT_AIR ) )
	{
		Unit *unitPtr = unit_array[*pointingRecno = locPtr->unit_recno(UNIT_AIR)];
		if( unitPtr->is_stealth() )
		{
			// no action, skip to next
		}
		else if( unitPtr->is_own() )
			return SCREEN_OBJECT_FRIEND_UNIT;
		else if( nation_array.player_recno && unitPtr->nation_recno
			&& (~nation_array)->get_relation_status(unitPtr->nation_recno) >= RELATION_FRIENDLY )
			return SCREEN_OBJECT_ALLIED_UNIT;
		else
			return SCREEN_OBJECT_ENEMY_UNIT;
	}
	
	if( locPtr->unit_recno( UNIT_LAND ) )
	{
		Unit *unitPtr = unit_array[*pointingRecno = locPtr->unit_recno(UNIT_LAND)];
		if( unitPtr->is_stealth() )
		{
			// no action, skip to next
		}
		else if( unitPtr->is_own() )
			return SCREEN_OBJECT_FRIEND_UNIT;
		else if( nation_array.player_recno && unitPtr->nation_recno
			&& (~nation_array)->get_relation_status(unitPtr->nation_recno) >= RELATION_FRIENDLY )
			return SCREEN_OBJECT_ALLIED_UNIT;
		else
			return SCREEN_OBJECT_ENEMY_UNIT;
	}
	
	if( locPtr->unit_recno( UNIT_SEA ) )
	{
		Unit *unitPtr = unit_array[*pointingRecno = locPtr->unit_recno(UNIT_SEA)];
		if( unitPtr->is_stealth() )
		{
			// no action, skip to next
		}
		else if( unitPtr->is_own() )
			return SCREEN_OBJECT_FRIEND_UNIT;
		else if( nation_array.player_recno && unitPtr->nation_recno
			&& (~nation_array)->get_relation_status(unitPtr->nation_recno) >= RELATION_FRIENDLY )
			return SCREEN_OBJECT_ALLIED_UNIT;
		else
			return SCREEN_OBJECT_ENEMY_UNIT;
	}
	
	// -------- check pointing at firm ---------//

	if( locPtr->is_firm() )
	{
		Firm *firmPtr = firm_array[*pointingRecno = locPtr->firm_recno()];
		if( firmPtr->is_stealth() )
		{
			// no action, skip to next
		}
		else if( firmPtr->is_own() )
			return SCREEN_OBJECT_FRIEND_FIRM;
		else if( nation_array.player_recno && firmPtr->nation_recno
			&& (~nation_array)->get_relation_status(firmPtr->nation_recno) >= RELATION_FRIENDLY )
			return SCREEN_OBJECT_ALLIED_FIRM;
		else
			return SCREEN_OBJECT_ENEMY_FIRM;
	}

	// ------- check pointing at town ---------//

	if( locPtr->is_town() )
	{
		Town *townPtr = town_array[*pointingRecno = locPtr->town_recno()];
		if( townPtr->is_stealth() )
		{
			// no action skip to next
		}
		else if( townPtr->is_own() )
			return SCREEN_OBJECT_FRIEND_TOWN;
		else if( nation_array.player_recno && townPtr->nation_recno
			&& (~nation_array)->get_relation_status(townPtr->nation_recno) >= RELATION_FRIENDLY )
			return SCREEN_OBJECT_ALLIED_TOWN;
		else
			return SCREEN_OBJECT_ENEMY_TOWN;
	}

	// -------- check pointing a site ---------//

	if( locPtr->has_site() )
	{
		Site *sitePtr = site_array[*pointingRecno = locPtr->site_recno()];
		if( sitePtr->is_stealth() )
		{
			// no action, skip to next
		}
		else
			return SCREEN_OBJECT_SITE;
	}

	// -------- check pointing a wall ---------//

	if( locPtr->is_wall() )
	{
		*pointingRecno = locPtr->power_nation_recno;
		return SCREEN_OBJECT_WALL;
	}

	return SCREEN_OBJECT_NONE;
}
// ---------- End of function Power::find_pointing_type --------//

