//Filename	  : OPOWER.CPP
//Description : Object Power

#include <OPOWER.H>
#include <OMOUSE.H>
#include <OMOUSECR.H>
#include <OBOX.H>
#include <OUNIT.H>
#include <OFIRM.H>
#include <OF_MAGI.H>
#include <OTOWN.H>
#include <OSITE.H>
#include <OSYS.H>
#include <OWAYPNT.H>
#include <ONATION.H>
#include <OWORLD.H>
#include <OWORLDMT.H>
#include <OINFO.H>
#include <OU_CARA.H>
#include <OSERES.H>
#include <OREMOTE.H> 
#include <OF_MFORT.H>
#include <OCONFIG.H>
#include <OTUTOR.H>
#include <OGAME.H>
#include <OSE.H>
#include <OSERES.H>

//----------- Begin of function Power::Power -----------//

Power::Power()
{
	memset( this, 0, sizeof(Power) );
}
//----------- End of function Power::Power -----------//


//----------- Begin of function Power::~Power -----------//

Power::~Power()
{
}
//----------- End of function Power::~Power -----------//


//----------- Begin of function Power::init -----------//

void Power::init()
{
	memset( this, 0, sizeof(Power) );

	reset_selection();		// there may be selections left by the previous game.
}
//----------- End of function Power::init -----------//


//-------- Begin of function Power::mouse_handler --------//
//
// React immediately when the mouse is moved or clicked.
//
void Power::mouse_handler()
{
	if( sys.view_mode != MODE_NORMAL )
	{
		mouse_cursor.set_icon(CURSOR_NORMAL);
		return;
	}

	if( !enable_flag || win_opened || Box::opened_flag ||
		 sys.signal_exit_flag  )		// a window or box is opened upon the current interface
	{
		return;
	}

	//---- if there is an object at where the mouse cursor is pointing at, change the cursor shape ----//

	Location *pointingLoc = NULL;
	short selectedRecno = 0;
	ScreenObjectType selectedType = find_selected_type(&selectedRecno);

	if( mouse.cur_x >= ZOOM_X1 && mouse.cur_x <= ZOOM_X2 &&
		 mouse.cur_y >= ZOOM_Y1 && mouse.cur_y <= ZOOM_Y2 &&
		 (pointingLoc = test_detect(mouse.cur_x, mouse.cur_y))!= NULL)
	{
		// ------- pointing at something -------//

		short pointingRecno;
		ScreenObjectType pointingType = find_pointing_type(pointingLoc, &pointingRecno);

		mouse_cursor.set_icon( choose_cursor(mouse.cur_x, mouse.cur_y,
			selectedType, selectedRecno, pointingType, pointingRecno) );
	}
	else
	{
		mouse_cursor.set_icon( choose_cursor(mouse.cur_x, mouse.cur_y,
			selectedType, selectedRecno, SCREEN_OBJECT_NONE, 0) );
	}

	//---- pressing right button in command mode -> cancel command mode ----//

	if( mouse.right_press && command_id )
	{
		command_id = 0;
		info.disp();
		return;
	}
}
//--------- End of function Power::mouse_handler ---------//



//-------- Begin of function Power::detect_frame --------//
//
// Detect selecting objects or laying tracks with the mouse
// selection frame.
//
// return: <int> 1 - objects selected or tracks built.
//					  0 - nothing selected or built.
//
int Power::detect_frame()
{
	//----- detect left mouse button to activate frame selection -----//

	int rc=0, selectedCount=0;

	if( mouse.is_mouse_event() )
	{
		if( mouse.mouse_event_type == LEFT_BUTTON &&
			 (game.game_mode != GAME_TUTORIAL ||
			  (game.game_mode == GAME_TUTORIAL && tutor.allow_mouse_click(mouse.click_x(LEFT_BUTTON), mouse.click_y(LEFT_BUTTON), 1, 1))))
		{
			int mouseX = mouse.click_x(LEFT_BUTTON);
			int mouseY = mouse.click_y(LEFT_BUTTON);

			if( !mouse_cursor.frame_flag )
			{
				if( mouseX >= ZOOM_X1 && mouseX <= ZOOM_X2 &&		// if the mouse is inside the zoom area
					 mouseY >= ZOOM_Y1 && mouseY <= ZOOM_Y2)
				{
					// ########## begin Gilbert 24/3 ########//
					//-------- set boundary of mouse -------//

					mouse.set_boundary(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2);

					// ########## end Gilbert 24/3 ########//

					if( command_id && execute_command(mouseX, mouseY) )
						return 1;

					mouse_cursor.set_frame(1, 0);					// set the frame on 
					mouse_cursor.process(mouseX, mouseY);
				}
			}

//			if( mouseX >= MAP_X1 && mouseX <= MAP_X2 &&		// if the mouse is inside the zoom area
//				 mouseY >= MAP_Y1 && mouseY <= MAP_Y2 )
//			{
//				int curXLoc;
//				int curYLoc;
//				char curMobileType;
//
//				if( world.map_matrix->get_detect_location(mouseX, mouseY, &curXLoc, &curYLoc, &curMobileType) )
//				{
//					mouse.set_boundary(MAP_X1, MAP_Y1, MAP_X2, MAP_Y2, 1);		// rhombus boundary
//				}
//			}

		}

		//------- the selection action is complete --------//

		else if( mouse.mouse_event_type == LEFT_BUTTON_RELEASE &&
			 (game.game_mode != GAME_TUTORIAL ||
			  (game.game_mode == GAME_TUTORIAL && tutor.allow_mouse_click(mouse.release_x(LEFT_BUTTON), mouse.release_y(LEFT_BUTTON), 2, 1))))
		{
			int mouseX = mouse.click_x(LEFT_BUTTON);
			int mouseY = mouse.click_y(LEFT_BUTTON);
			int mouseReleaseX = mouse.release_x(LEFT_BUTTON);
			int mouseReleaseY = mouse.release_y(LEFT_BUTTON);

			//-------- reset boundary of mouse -------//

			mouse.reset_boundary();

			if( mouse_cursor.frame_flag )
			{
				mouse_cursor.process(mouseReleaseX, mouseReleaseY);
				mouse_cursor.set_frame(0);

				rc = detect_select( mouse_cursor.frame_x1, mouse_cursor.frame_y1,
										  mouse_cursor.frame_x2, mouse_cursor.frame_y2,
										  0, mouse.event_skey_state & SHIFT_KEY_MASK);
			}
		}
	}
	else
	{
		// no mouse event but keep pressing left button

		if( mouse.left_press )
		{
			if( mouse_cursor.frame_flag )
			{
				mouse_cursor.process(mouse.cur_x, mouse.cur_y);
			}
		}
		else
		{
			mouse_cursor.set_frame(0);
		}
	}

	return rc;
}
//--------- End of function Power::detect_frame ---------//


//-------- Begin of function Power::detect_action --------//
//
// return: <int> 1 - action executed.
//					  0 - no action ordered.
//
int Power::detect_action()
{
	if( !mouse.has_mouse_event || mouse.mouse_event_type != RIGHT_BUTTON || !nation_array.player_recno)
		return 0;

	int curXLoc, curYLoc;
	char curMobileType;
	int mouseX = mouse.click_x(RIGHT_BUTTON);
	int mouseY = mouse.click_y(RIGHT_BUTTON);

	if(game.game_mode == GAME_TUTORIAL && !tutor.allow_mouse_click(mouseX, mouseY, 2, 2))
		return 0;
	
	//--------- if click on the zoom window --------//

	if( mouseX >= ZOOM_X1 && mouseX <= ZOOM_X2 &&		// if the mouse is inside the zoom area
		 mouseY >= ZOOM_Y1 && mouseY <= ZOOM_Y2 &&
		 ( test_detect(mouseX, mouseY, &curMobileType, &curXLoc, &curYLoc )
		  || world.zoom_matrix->get_detect_location(mouseX, mouseY, &curXLoc, &curYLoc, &curMobileType)) )
	{
	}

	//---------- if click on the map window -----------//

	else if( world.map_matrix->get_detect_location( mouseX, mouseY,
		&curXLoc, &curYLoc, &curMobileType) )			// if the mouse is inside the zoom area
	{
	}

	else
	{
		return 0;
	}

	//-------- unit action ----------//

	if( unit_array.selected_recno && unit_array[unit_array.selected_recno]->is_own() )
	{
		unit_group.set_to_selected();
		// ####### begin Gilbert 20/11 #######//
		unit_group.right_click(curXLoc, curYLoc, COMMAND_PLAYER, 
			(mouse.event_skey_state & ALT_KEY_MASK) != 0,		// alt to add way point
			(mouse.event_skey_state & SHIFT_KEY_MASK) != 0,	// shift key to force attack
			mouse.click_count(RIGHT_BUTTON) > 1 );			// double click to force move
		// ####### end Gilbert 20/11 #######//
		return 1;
	}

	//-------- firm action ----------//

	if( firm_array.selected_recno && firm_array[firm_array.selected_recno]->is_own() )
	{
		firm_array[firm_array.selected_recno]->right_click(curXLoc, curYLoc);
		return 1;
	}

	//-------- town action ----------//

	if( town_array.selected_recno && town_array[town_array.selected_recno]->is_own() )
	{
		town_array[town_array.selected_recno]->right_click(curXLoc, curYLoc);
		return 1;
	}

	return 0;
}
//-------- End of function Power::detect_action --------//


//------ Begin of function Power::test_detect -------//
//
// Test detect if there is an object at where the mouse cursor
// is currently pointing at.
//
// return: <Location*> The pointer to the location with a sprite or firm
//							  being detected.
//					  		  NULL - no sprite detected.
// mobileType          UNIT_AIR or UNIT_LAND (don't return UNIT_SEA)
//
// lock_unit_recno like as follow :
// if you press ctrl, the first unit hit is locked
// test_detect keep on returning the location of the unit, even outside the screen
// unit ctrl key release or unit disappeared
//
//
Location* Power::test_detect(int scrnX, int scrnY, char *mobileType, int *retXLoc, int *retYLoc)
{
	char dummy, tempMobileType;
	if( !mobileType )
		mobileType = &dummy;

	int dummyX, dummyY;
	if( !retXLoc )
		retXLoc = &dummyX;
	if( !retYLoc )
		retYLoc = &dummyY;

	if( locked_unit_recno )
	{
		Unit *lockedUnit;
		if( (mouse.skey_state & CONTROL_KEY_MASK)
			&& !unit_array.is_deleted(locked_unit_recno)
			&& (lockedUnit = unit_array[locked_unit_recno])
			&& lockedUnit->is_visible()
			&& !lockedUnit->is_stealth() )
		{
			// empty
			*mobileType = lockedUnit->mobile_type;
			return world.get_loc( (*retXLoc = lockedUnit->next_x_loc()), (*retYLoc = lockedUnit->next_y_loc()) );
		}
		else
		{
			locked_unit_recno = 0;
		}
	}

	//---- only proceed if the mouse cursor is inside the zoom map area ---//

	if( scrnX < world.zoom_matrix->image_x1 || scrnX > world.zoom_matrix->image_x2 ||		// if the mouse is inside the zoom area
		 scrnY < world.zoom_matrix->image_y1 || scrnY > world.zoom_matrix->image_y2 )
	{
		return NULL;
	}

	//--------------------------------------------//

	#define DETECT_SPREAD 4

	int detectSpread = mouse.skey_state & CAP_LOCK_STATE_MASK ? 0 : DETECT_SPREAD;
//	int detectSpread = DETECT_SPREAD;
	Location* retLoc = NULL;
	int curXLoc, curYLoc;

	if( world.zoom_matrix->get_detect_location(scrnX, scrnY, &curXLoc, &curYLoc, &tempMobileType)
		|| (detectSpread && world.zoom_matrix->get_close_location(scrnX, scrnY, &curXLoc, &curYLoc, &tempMobileType)) )
	{
		int selXLoc1=curXLoc-detectSpread;   	// expand 4 tiles in case of big sprite
		if( selXLoc1 < 0 )
			selXLoc1 = 0;

		int selYLoc1=curYLoc-detectSpread;
		if( selYLoc1 < 0 )
			selYLoc1 = 0;

		int selXLoc2=curXLoc+detectSpread;
		if( selXLoc2 >= MAX_WORLD_X_LOC )
			selXLoc2 = MAX_WORLD_X_LOC-1;

		int selYLoc2=curYLoc+detectSpread;
		if( selYLoc2 >= MAX_WORLD_Y_LOC)
			selYLoc2 = MAX_WORLD_Y_LOC-1;

		//---------- select sprite --------------//

		int		rowXLoc, rowYLoc;
		int 		xLoc, yLoc;
		int		absX = scrnX - world.zoom_matrix->image_x1 + world.zoom_matrix->get_base_x();
		int		absY = scrnY - world.zoom_matrix->image_y1 + world.zoom_matrix->get_base_y();
		int		minFromTop = 2 * VGA_WIDTH + VGA_HEIGHT;	// any large value
		Location *locPtr;

      //-----------------------------------------//
		// First pass : scan air unit
		//
		// [1,1] [2,2] [3,3] (scan traversal)
		// [2,1] [3,2] [4,3]
		// [3,1] [4,2] [5,2]
		// [4,1] [5,1] [6,1]
		//-----------------------------------------//

		rowXLoc = selXLoc1;
		rowYLoc = selYLoc1;
		// minFromTop = 2 * VGA_WIDTH + VGA_HEIGHT;
		for( ; rowYLoc<=selYLoc2 && rowXLoc<=selXLoc2; rowYLoc<selYLoc2 ? ++rowYLoc : ++rowXLoc )
		{
			xLoc = rowXLoc;
			yLoc = rowYLoc;
			for( ; xLoc<=selXLoc2 && yLoc>=selYLoc1; ++xLoc, --yLoc )
			{
				locPtr = world.get_loc(xLoc, yLoc);
				Unit *unitPtr;
				if( locPtr->unit_recno(UNIT_AIR) && !unit_array.is_deleted(locPtr->air_cargo_recno) )	// if the mouse does not click directly on a sprite
				{
					unitPtr = unit_array[locPtr->air_cargo_recno];
					tempMobileType = UNIT_AIR;
					err_when(!unitPtr->is_visible());
					err_when( unitPtr->mobile_type != UNIT_AIR );
				}
				else
					unitPtr = NULL;

				//----- there is a sprite in the location -----//

				if( unitPtr && !unitPtr->is_stealth() )
				{
					unitPtr->update_abs_pos();

					if( absX >= unitPtr->abs_x1 && absY >= unitPtr->abs_y1 &&
						 absX <= unitPtr->abs_x2 && absY <= unitPtr->abs_y2 )
					{
						int curZ = unitPtr->get_z();
						int logAbsX = world.zoom_matrix->calc_abs_x(unitPtr->cur_x, unitPtr->cur_y, curZ);
						int logAbsY = world.zoom_matrix->calc_abs_y(unitPtr->cur_x, unitPtr->cur_y, curZ)
							+(unitPtr->loc_width*ZOOM_LOC_X_HEIGHT+unitPtr->loc_height*ZOOM_LOC_Y_HEIGHT)/2
							+ unitPtr->sprite_info->max_height;

						// calc distace from center
						// distance from the middle top in screen y axis
						// and center of left and right in screen x axis
						// weight of x : y difference is 2 : 1
						int fromTop = abs( absX - logAbsX ) * 2
							+ abs( absY - logAbsY );

						if( fromTop <= minFromTop )	// allow overwrite on bottom unit
						{
							minFromTop = fromTop;
							retLoc = locPtr;
							*mobileType = tempMobileType;
							*retXLoc = xLoc;
							*retYLoc = yLoc;
							// set locked unit
							if( mouse.skey_state & CONTROL_KEY_MASK )
								locked_unit_recno = unitPtr->sprite_recno;
						}
					}
				}
			}
		}

		if( retLoc )
			return retLoc;

		// -------- second pass : land unit --------- //

		rowXLoc = selXLoc1;
		rowYLoc = selYLoc1;
		// minFromTop = 2 * VGA_WIDTH + VGA_HEIGHT;
		for( ; rowYLoc<=selYLoc2 && rowXLoc<=selXLoc2; rowYLoc<selYLoc2 ? ++rowYLoc : ++rowXLoc )
		{
			xLoc = rowXLoc;
			yLoc = rowYLoc;
			for( ; xLoc<=selXLoc2 && yLoc>=selYLoc1; ++xLoc, --yLoc )
			{
				locPtr = world.get_loc(xLoc, yLoc);
				Unit *unitPtr;
				if( (locPtr->unit_recno(UNIT_LAND) || locPtr->unit_recno(UNIT_SEA) )
					&& !unit_array.is_deleted(locPtr->cargo_recno) )	// if the mouse does not click directly on a sprite
				{
					unitPtr = unit_array[locPtr->cargo_recno];
					tempMobileType = UNIT_LAND;
					err_when(!unitPtr->is_visible());
					err_when( unitPtr->mobile_type == UNIT_AIR );
				}
				else
					unitPtr = NULL;

				//----- there is a sprite in the location -----//

				if( unitPtr && !unitPtr->is_stealth() )
				{
					unitPtr->update_abs_pos();

					if( absX >= unitPtr->abs_x1 && absY >= unitPtr->abs_y1 &&
						 absX <= unitPtr->abs_x2 && absY <= unitPtr->abs_y2 )
					{
						int curZ = unitPtr->get_z();
						int logAbsX = world.zoom_matrix->calc_abs_x(unitPtr->cur_x, unitPtr->cur_y, curZ);
						int logAbsY = world.zoom_matrix->calc_abs_y(unitPtr->cur_x, unitPtr->cur_y, curZ)
							+(unitPtr->loc_width*ZOOM_LOC_X_HEIGHT+unitPtr->loc_height*ZOOM_LOC_Y_HEIGHT)/2
							+ unitPtr->sprite_info->max_height;

						// calc distace from center
						// distance from the middle top in screen y axis
						// and center of left and right in screen x axis
						// weight of x : y difference is 2 : 1
						int fromTop = abs( absX - logAbsX ) * 2
							+ abs( absY - logAbsY );

						if( fromTop <= minFromTop )	// allow overwrite on bottom unit
						{
							minFromTop = fromTop;
							retLoc = locPtr;
							*mobileType = tempMobileType;
							*retXLoc = xLoc;
							*retYLoc = yLoc;
							// set locked unit
							if( mouse.skey_state & CONTROL_KEY_MASK )
								locked_unit_recno = unitPtr->sprite_recno;
						}
					}
				}
			}
		}

		if( retLoc )
			return retLoc;

		// -------- third pass : scan land town/wall --------//

		locPtr = world.get_loc(curXLoc, curYLoc);
		if( locPtr->is_town() && !town_array[locPtr->town_recno()]->is_stealth()
			|| locPtr->is_firm() && !firm_array[locPtr->firm_recno()]->is_stealth()
			|| locPtr->has_site() && !site_array[locPtr->site_recno()]->is_stealth() 
			|| locPtr->is_wall() )
		{
			*mobileType = UNIT_LAND;
			*retXLoc = curXLoc;
			*retYLoc = curYLoc;
			retLoc = locPtr;
		}

		if( retLoc )
			return retLoc;
	}

	return retLoc;
}
//------ End of function Power::test_detect -------//


//------ Begin of function Power::detect_select -------//
//
// This function is called when the mouse has been clicked,
// this function will select the objects inside the mouse selection area,
// and deselect previously selected objects.
//
// <int> selX1, selY1, selX2, selY2 - the positions of the selection box
//
// <int> shiftSelect - add/remove individual selected unit
//
// return : <int> >0 - no. of units or firms selected
//						0 	- no unit or firm detected in the rectd area.
//
int Power::detect_select(int selX1, int selY1, int selX2, int selY2, int recallGroup, int shiftSelect)
{
	if( selX1 > selX2 )
	{
		int temp = selX2;
		selX2 = selX1;
		selX1 = temp;
	}

	if( selY1 > selY2 )
	{
		int temp = selY2;
		selY2 = selY1;
		selY1 = temp;
	}

	//------ calc absolute positions for fast comparsion ---//

	int absSelX1 = selX1 - world.zoom_matrix->image_x1 + world.zoom_matrix->get_base_x();	// the mouse cursor's absolute position on the whole world map
	int absSelY1 = selY1 - world.zoom_matrix->image_y1 + world.zoom_matrix->get_base_y();
	int absSelX2 = absSelX1 - selX1 + selX2;
	int absSelY2 = absSelY1 - selY1 + selY2;

	int selectOneOnly = (selX2-selX1)<=3 && (selY2-selY1)<=3;

	char pMobileType;
	int testDetectXLoc, testDetectYLoc;
	Location *testDetectLoc = selectOneOnly ? test_detect( selX2, selY2, &pMobileType, &testDetectXLoc, &testDetectYLoc ) : NULL;
	int   rc=0;

	//------------ if only select one unit -------------//

	if( selectOneOnly )
	{
		Location *locPtr = testDetectLoc;

		if( !locPtr )
			return 0;

		//--------- handle unit selection -------//

		if( locPtr->unit_recno(pMobileType) && !unit_array.is_deleted(locPtr->unit_recno(pMobileType)) )
		{
			Unit* unitPtr = unit_array[locPtr->unit_recno(pMobileType)];

			//-------- if recall a defined group ----------//

			if( recallGroup )
			{
				if( unitPtr->is_own() && unitPtr->in_team() )
				{
					// ###### begin Gilbert 1/2 ##### //
					if( !shiftSelect || !unit_array.selected_recno )		// if selecting firm/town... still reset selection too
					// ###### end Gilbert 1/2 ##### //
						power.reset_selection();

					unitPtr->select_team();
					rc = 1;
					
					if( se_res.mark_select_object_time() )
					{
						Unit *headUnit = unit_array[unit_array.selected_recno];
						se_res.sound( headUnit->cur_x_loc(), headUnit->cur_y_loc(), 1, 'S',
							headUnit->sprite_id, "SEL");
					}
				}
			}
			else
			{
				// ###### begin Gilbert 1/2 ##### //
				if( !shiftSelect || !unit_array.selected_recno )		// if selecting firm/town... still reset selection too
				// ###### end Gilbert 1/2 ##### //
					power.reset_selection();

				unitPtr->left_click(shiftSelect);
				rc = 1;
				if( se_res.mark_select_object_time() && unitPtr->is_own() )
				{
					se_res.sound( unitPtr->cur_x_loc(), unitPtr->cur_y_loc(), 1, 'S',
						unitPtr->sprite_id, "SEL");		
				}
			}
		}

		//----- the following lines only apply to recallGroup ----//

		if( !recallGroup && pMobileType != UNIT_AIR )
		{
			//--------- handle firm selection -------//

			if( locPtr->is_firm() && !firm_array.is_deleted(locPtr->firm_recno()) )
			{
				Firm* firmPtr = firm_array[locPtr->firm_recno()];

				power.reset_selection();
				firmPtr->left_click();

				rc = 1;
				if( se_res.mark_select_object_time() )
				{
					se_res.sound(firmPtr->center_x, firmPtr->center_y, 1,
						'F', firmPtr->firm_id, firmPtr->under_construction ? "SELU" : "SEL" );
					se_res.sound(firmPtr->center_x, firmPtr->center_y, 1,
						'B', firmPtr->firm_build_id, firmPtr->under_construction ? "SELU" : "SEL" );
				}
			}

			//--------- handle town selection -------//

			else if( locPtr->is_town() && !town_array.is_deleted(locPtr->town_recno()) )
			{
				Town* townPtr = town_array[locPtr->town_recno()];

				power.reset_selection();
				townPtr->left_click();

				rc = 1;
				if( se_res.mark_select_object_time() )
				{
					se_res.sound(townPtr->center_x, townPtr->center_y, 1,
					'T', 0, "SEL" );
				}
			}

			//--------- handle site selection ------//

			// ##### begin Gilbert 3/11 ######//
			else if( !rc && locPtr->has_site() && !locPtr->is_firm() )
			// ##### end Gilbert 3/11 ######//
			{
				power.reset_selection();
				site_array.selected_recno = locPtr->site_recno();

				rc = 1;
			}
		}
	}
	else //------- if select more than one unit ---------//
	{
		Unit* unitPtr;

		int selectSound = 0;
		for( int unitRecno = unit_array.size(); unitRecno > 0; --unitRecno )
		{
			if( !unit_array.is_deleted(unitRecno)
				 && (unitPtr = unit_array[unitRecno])->is_visible()
				 && unitPtr->is_own() )
			{
				unitPtr->update_abs_pos();

				if( m.is_touch( absSelX1, absSelY1, absSelX2, absSelY2,
					 unitPtr->abs_x1, unitPtr->abs_y1,
					 unitPtr->abs_x2, unitPtr->abs_y2 ) )
				{
					// ####### begin Gilbert 4/2 ##########//
					if( !rc && (!shiftSelect || !unit_array.selected_recno || !unit_array[unit_array.selected_recno]->is_own() ) )		// if selecting firm/town... still reset selection too
					// ####### end Gilbert 4/2 ##########//
					{
						power.reset_selection();
						if( se_res.mark_select_object_time() && !selectSound)
						{
							se_res.sound( unitPtr->cur_x_loc(), unitPtr->cur_y_loc(), 1, 'S',
								unitPtr->sprite_id, "SEL");
							selectSound = 1;
						}
					}
					else					
					if( se_res.mark_select_object_time() && !selectSound)
					{
						Unit *headUnit = unit_array[unit_array.selected_recno];
						se_res.sound( headUnit->cur_x_loc(), headUnit->cur_y_loc(), 1, 'S',
							headUnit->sprite_id, "SEL");
						selectSound = 1;
					}
					// ##### begin Gilbert 4/2 ######//
					// unitPtr->left_click(shiftSelect);
					unitPtr->left_click(0);			// do not pass shiftSelect, otherwise it will be toggle select
					// ##### end Gilbert 4/2 ######//
					rc = 1;
				}
			}
		}
	}

	//--------------------------------------//

	if( rc )
	{
		reset_command();		// reset current command when a new unit is selected
		info.disp();
	}

	return rc;
}
//------ End of function Power::detect_select -------//


//--------- Begin of function Power::reset_selection ---------//
//
// Reset selection.
//
void Power::reset_selection()
{
	int i;

	//----- reset unit selection -------//

	for(i=1; i <=unit_array.size() ; i++)
	{
		if( unit_array.is_truly_deleted(i) )
			continue;

		unit_array[i]->selected_flag = 0;
	}

	unit_array.selected_recno = 0;
	unit_array.selected_count = 0;

	//---- reset other selection --------//

	firm_array.selected_recno = 0;
	town_array.selected_recno = 0;
	site_array.selected_recno = 0;

	reset_command();		// reset current command when a new unit is selected
}
//----------- End of function Power::reset_selection -----------//


//--------- Begin of function Power::issue_command ---------//
//
// <int> commandId  		  - the id. of the command
// [int] commandUnitRecno - the id. of the unit that issues the command
// [int] commandPara  	  - an extra parameter of the command
//
void Power::issue_command(int commandId, int commandUnitRecno, int commandPara)
{
	command_id    		 = commandId;
	command_unit_recno = command_firm_recno = 0;
	if( command_id >= COMMAND_UNIT_FIRST && command_id < COMMAND_UNIT_LAST )
		command_unit_recno = commandUnitRecno;
	else if( command_id >= COMMAND_FIRM_FIRST && command_id < COMMAND_FIRM_LAST )
		command_firm_recno = commandUnitRecno;

	command_para       = commandPara;
}
//----------- End of function Power::issue_command -----------//


//--------- Begin of function Power::execute_command ---------//
//
int Power::execute_command(int mouseX, int mouseY)
{
	if( (unit_array.is_deleted(command_unit_recno) 
		|| !unit_array[command_unit_recno]->is_visible() )
		&& firm_array.is_deleted(command_firm_recno) )
	{
		return 0;
	}

	Unit* cmdUnit = NULL;
	Firm* cmdFirm = NULL;
	if( command_unit_recno )
		cmdUnit = unit_array[command_unit_recno];
	if( command_firm_recno )
		cmdFirm = firm_array[command_firm_recno];

	//--------- get the clicked location --------//

	int curXLoc;
	int curYLoc;
	char curMobileType;

	if( !world.zoom_matrix->get_detect_location(mouseX, mouseY, &curXLoc, &curYLoc, &curMobileType) )
		return 0;

	Location* locPtr = world.get_loc(curXLoc, curYLoc);

	//-------- set boundary of mouse -------//

	mouse.set_boundary(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2);

	int shiftSelect = mouse.event_skey_state & SHIFT_KEY_MASK;

	//------- activate frame selection --------//

	err_when( command_id >= COMMAND_UNIT_FIRST && command_id < COMMAND_UNIT_LAST && !cmdUnit );
	err_when( command_id >= COMMAND_FIRM_FIRST && command_id < COMMAND_FIRM_LAST && !cmdFirm );

	int rc=0;
	int nextCommandId = 0;

	switch(command_id)
	{
		case COMMAND_BUILD_FIRM:
			se_ctrl.immediate_sound("CONSTR");
			cmdUnit->sound("ACK");
			// ######### begin Gilbert 4/3 #######//
			// ------ snap to raw -------//
			if( locPtr->has_site() )
			{
				Site *sitePtr = site_array[locPtr->site_recno()];
				if( sitePtr->site_type == SITE_RAW )
				{
					curXLoc = sitePtr->map_x_loc;
					curYLoc = sitePtr->map_y_loc;
				}
			}
			// ######### end Gilbert 4/3 #######//
			cmdUnit->build_firm(curXLoc, curYLoc, command_para, COMMAND_PLAYER);

			rc = 1;
			info.disp();
			break;

		case COMMAND_TRANSFORM_FORTRESS:
			cmdUnit->sound("ACK");
			if( unit_array.selected_recno && unit_array[unit_array.selected_recno]->is_own() )
			{
				unit_group.set_to_selected();
				// ####### begin Gilbert 25/1 ########//
				unit_group.transform_mfort( curXLoc, curYLoc, COMMAND_PLAYER );
				// ####### end Gilbert 25/1 ########//
			}
			rc = 1;
			info.disp();	
			break;

		case COMMAND_SETTLE:
			cmdUnit->sound("ACK");

			//-- if there is an existing town on the targetted location, group assign the selected units to it. Otherwise, order the selected unit to build a town first. --//

			if( locPtr->is_town() &&
				 town_array[locPtr->town_recno()]->can_accept_assign(
				 cmdUnit->sprite_recno, nation_array.player_recno) )
			{
				unit_group.set_to_selected();
				unit_group.assign(curXLoc, curYLoc, UNIT_LAND, COMMAND_PLAYER, nation_array.player_recno);				
			}
			else
			{
				cmdUnit->settle_town(curXLoc, curYLoc, COMMAND_PLAYER);
			}

			rc = 1;
			info.disp();
			break;

		case COMMAND_SET_CARAVAN_STOP:
			{
				UnitCaravan* unitCaravan = cmdUnit->cast_to_UnitCaravan();

				err_when( !unitCaravan );

				if( locPtr->is_firm() )
				{
					Firm* firmPtr = firm_array[locPtr->firm_recno()];

					if( unitCaravan->can_set_stop(firmPtr->firm_recno) )
					{
						unitCaravan->sound("ACK");
						unitCaravan->set_stop(command_para, curXLoc, curYLoc, COMMAND_PLAYER);		// command_para is the id. of the stop
					}
				}

				rc = 1;
				break;
			}

		case COMMAND_GOD_CAST_POWER:
			cmdUnit->sound("ACK");
			cmdUnit->go_cast_power(curXLoc, curYLoc, COMMAND_PLAYER);

			rc = 1;
			break;

		// ####### begin Gilbert 5/3 #########//
		case COMMAND_USE_ITEM_ON_UNIT:
		case COMMAND_USE_ITEM_ON_TOWN:
			{
				int targetBaseObjRecno;
				if( (targetBaseObjRecno = world.get_loc(curXLoc, curYLoc)->base_obj_recno())
					&& !base_obj_array.is_deleted(targetBaseObjRecno) )
				{
					BaseObj *targetBaseObj = base_obj_array[targetBaseObjRecno];
					Item *itemPtr = &cmdUnit->item;
					if( itemPtr->id == command_para		// make sure the same item
						&& itemPtr->can_use_manually() > 0
						&& ( COMMAND_USE_ITEM_ON_UNIT && targetBaseObj->cast_to_Unit() 
						|| COMMAND_USE_ITEM_ON_TOWN && targetBaseObj->cast_to_Town()) )
					{
						if( !remote.is_enable() )
						{
							itemPtr->use_manually( command_unit_recno, targetBaseObjRecno );
						}
						else
						{
							short *shortPtr = (short *)remote.new_send_queue_msg( MSG_UNIT_USE_ITEM, 3*sizeof(short) );
							// packet structure : <sprite recno> <target base obj recno> <0>
							*shortPtr = command_unit_recno;
							shortPtr[1] = targetBaseObjRecno;
							shortPtr[2] = 0;
						}
					}
				}
				rc = 1;
			}
			break;

		case COMMAND_USE_ITEM_ON_SPACE:
			{
				Item *itemPtr = &cmdUnit->item;
				if( itemPtr->id == command_para		// make sure the same item
					&& itemPtr->can_use_manually() > 0
					&& locPtr->explored() )
				{
					if( !remote.is_enable() )
					{
						itemPtr->use_manually( command_unit_recno, curXLoc, curYLoc );
					}
					else
					{
						short *shortPtr = (short *)remote.new_send_queue_msg( MSG_UNIT_USE_ITEM, 3*sizeof(short) );
						// packet structure : <sprite recno> <xLoc> <yLoc>
						*shortPtr = command_unit_recno;
						shortPtr[1] = curXLoc;
						shortPtr[2] = curYLoc;
					}
				}
				rc = 1;
			}
			break;
		// ####### end Gilbert 5/3 #########//

		case COMMAND_FIRM_MAGIC:
			if( cmdFirm->cast_to_FirmMagic() )
				cmdFirm->cast_to_FirmMagic()->cast_magic( command_para, curXLoc, curYLoc, 0, COMMAND_PLAYER );

			info.disp();
			rc = 1;
			break;

		case COMMAND_MAGIC_TELEPORT_PRE:
			if( cmdFirm->cast_to_FirmMagic() )
			{
				int pointedUnitRecno;
				int pointedMobileType;
				if( locPtr->has_any_unit() && (pointedUnitRecno = locPtr->get_any_unit(pointedMobileType)) 
					&& cmdFirm->cast_to_FirmMagic()->set_teleport_unit(pointedUnitRecno) )
				{
					nextCommandId = COMMAND_MAGIC_TELEPORT_POST;
				}

				rc = 1;
			}
			// info.disp();		// don't call info.disp yet
			break;

		case COMMAND_MAGIC_TELEPORT_POST:
			if( cmdFirm->cast_to_FirmMagic() )
			{
				cmdFirm->cast_to_FirmMagic()->cast_magic( command_para, curXLoc, curYLoc, 
					cmdFirm->cast_to_FirmMagic()->teleport_unit_recno, COMMAND_PLAYER );
			}
			info.disp();
			rc = 1;
			break;
	}

	if( rc )
		// command_id = 0;
		command_id = nextCommandId;

	return rc;
}
//----------- End of function Power::execute_command -----------//
