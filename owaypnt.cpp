//Filename   : OWAYPNT.CPP
//Description: class WayPoint

#include <OVGA.H>
#include <OMODEID.H>
#include <OBITMAP.H>
#include <OIMGRES.H>
#include <OWORLDMT.H>
#include <OANLINE.H>
#include <OPFIND.H>
#include <OWAYPNT.H>


//--------- Begin of function WayPoint::WayPoint ---------//
//
WayPoint::WayPoint() : DynArray(sizeof(OneWayPoint), 20)
{
	//---- set the cur_unit_group to the currently selected units ----//

	cur_unit_group.set_to_selected();

	//---- enable has_way_point of all units in the group ----//

	for( int i=cur_unit_group.size() ; i>0 ; i-- )
	{
		cur_unit_group.get_unit(i)->has_way_point = true;		// it will be reset to false in Unit::stop_move()
	}
}
//----------- End of function WayPoint::WayPoint ---------//


//--------- Begin of function WayPoint::add ---------//
//
void WayPoint::add(int xLoc, int yLoc)
{
	OneWayPoint oneWayPoint;

	oneWayPoint.loc_x = xLoc;
	oneWayPoint.loc_y = yLoc;

	linkin(&oneWayPoint);
}
//----------- End of function WayPoint::add ---------//


//--------- Begin of function WayPoint::del ---------//
//
void WayPoint::del(int delRecno)
{
	linkout(delRecno);
}
//----------- End of function WayPoint::del ---------//


//------- Begin of function WayPoint::get -----//

OneWayPoint* WayPoint::get(int recNo)
{
	OneWayPoint* oneWayPoint = (OneWayPoint*) DynArray::get(recNo);

	if( !oneWayPoint )
		err.run( "WayPoint[] is deleted" );

	return oneWayPoint;
}
//--------- End of function WayPoint::get ----//


//--------- Begin of function WayPoint::process ---------//
//
// return: <int> 1 - process okay
//					  0 - all way points are finished, delete this WayPoint.
//
int WayPoint::process()
{
	//------- check the cur_unit_group -------//

	for( int i=cur_unit_group.size() ; i>0 ; i-- )
	{
		//--- if the unit is no longer valid, remove it from cur_unit_group ---//

		if( !cur_unit_group.is_visible(i) )
		{
			cur_unit_group.del_unit(i);
			continue;
		}

		//--- if the unit was ordered to do something else, remove it from cur_unit_group. (has_way_point is reset in UnitGroup::right_click() ) ---//

		Unit* unitPtr = cur_unit_group.get_unit(i);

		if( !unitPtr->has_way_point )
		{
			cur_unit_group.del_unit(i);
			continue;
		}

		if( !unitPtr->is_all_stop() )
			break;
	}

	if( i>0 )		// if some units are still moving, do nothing
		return 1;

	//--- if all units in the group has stopped, order them to move to new location ----//

	if( size() > 0 && cur_unit_group.size() > 0 )
	{
		OneWayPoint* oneWayPoint = get(1);

		cur_unit_group.right_click(oneWayPoint->loc_x, oneWayPoint->loc_y, COMMAND_AUTO, false, false, false );

		del(1);		// remove the one way point after successfully order the units to move

		return 1;
	}
	else		//-- no more way points and all units have stopped, now remove the WayPoint
	{
		return 0;
	}
}
//----------- End of function WayPoint::process ---------//


//--------- Begin of function WayPoint::draw ---------//
//
// <int> drawMode - 1 - draw on the mini map.
//						  2 - draw on the zoom map.
//
void WayPoint::draw(int drawMode)
{
	cur_unit_group.validate();		// remove deleted units from cur_unit_group.

	//--- calculate the center location of the units in the group ---//

	int srcXLocSum=0, srcYLocSum=0;
	int destXLocSum=0, destYLocSum=0;
	int unitCount = cur_unit_group.size();
	int i;

	for( i = 1 ; i<=unitCount ; i++ )
	{
		if( !cur_unit_group.is_visible(i) )
			continue;

		Unit* unitPtr = cur_unit_group.get_unit(i);
	
		srcXLocSum += unitPtr->next_x_loc();
		srcYLocSum += unitPtr->next_y_loc();

		destXLocSum += unitPtr->move_to_loc_x;
		destYLocSum += unitPtr->move_to_loc_y;
	}

	//------- draw the way point lines -----//
	for( i = 1 ; i<=unitCount ; i++ )
	{
		Unit* unitPtr = cur_unit_group.get_unit(i);
		if( unitPtr && unitPtr->selected_flag )
			break;
	}

	if (i == (unitCount + 1))
		return;
	//------- draw the way point lines -----//

	int srcXLoc  = srcXLocSum / unitCount;
	int srcYLoc  = srcYLocSum / unitCount;
	int destXLoc = destXLocSum / unitCount;
	int destYLoc = destYLocSum / unitCount;
	int srcX, srcY, destX, destY;

	for( i=0 ; i<=size() ; i++ )
	{
		if( i>0 )		// when i==0, we use the original destXLoc & destYLoc set in the above code
		{
			OneWayPoint* oneWayPoint = get(i);

			destXLoc = oneWayPoint->loc_x;
			destYLoc = oneWayPoint->loc_y;
		}

		if( drawMode==1 )
		{
			srcX  = world.map_matrix->calc_loc_pos_x(srcXLoc, srcYLoc) + MAP_X1;
			srcY  = world.map_matrix->calc_loc_pos_y(srcXLoc, srcYLoc) + MAP_Y1;
			destX = world.map_matrix->calc_loc_pos_x(destXLoc, destYLoc) + MAP_X1;
			destY = world.map_matrix->calc_loc_pos_y(destXLoc, destYLoc) + MAP_Y1;
		}
		else
		{
			world.zoom_matrix->get_zoom_pos(srcXLoc, srcYLoc, srcX, srcY);
			world.zoom_matrix->get_zoom_pos(destXLoc, destYLoc, destX, destY);
		}

		if( i>=1 )		// don't draw the first line
		{
			if( drawMode== 1 )
				vga_back.line(srcX, srcY, destX, destY, V_BLACK);
			else
				anim_line.draw_line(&vga_back, srcX, srcY, destX, destY, 0, 2);
		}

		if( drawMode==2 )		// draw on the zoom map
		{
			char *chPtr = image_icon.get_ptr("WAYPOINT");
			// ####### begin Gilbert 6/3 #######//
			// short chOffsetX = - (*(short *)chPtr ) + 7;
			// short chOffsetY = - (*(1+(short *)chPtr) ) -1;
			short chOffsetX = - ((Bitmap *)chPtr)->get_width() + 7;
			short chOffsetY = - ((Bitmap *)chPtr)->get_height() -1;
			// ####### end Gilbert 6/3 #######//

			world.zoom_matrix->put_bitmap_clip(destX+chOffsetX, destY+chOffsetY, chPtr);
		}

		srcXLoc = destXLoc;
		srcYLoc = destYLoc;
	}
}
//----------- End of function WayPoint::draw ---------//


//===============================================================//


//--------- Begin of function WayPointArray::WayPointArray ---------//
//
WayPointArray::WayPointArray() : DynArray(sizeof(WayPoint*), 20)
{
}
//----------- End of function WayPointArray::WayPointArray ---------//


//--------- Begin of function WayPointArray::init ---------//
//
void WayPointArray::init()
{
}
//----------- End of function WayPointArray::init ---------//


//--------- Begin of function WayPointArray::deinit ---------//
//
void WayPointArray::deinit()
{
	for( int i=size() ; i>0 ; i-- )
	{
		del(i);
	}
}
//----------- End of function WayPointArray::deinit ---------//


//--------- Begin of function WayPointArray::add_point ---------//
//
void WayPointArray::add_point(int xLoc, int yLoc)
{
	if( !unit_array.selected_recno )
		return;

	//---- see if the selected unit has a way point setting already ----//

	WayPoint* wayPoint;

	for( int i=size() ; i>0 ; i-- )
	{
		wayPoint = get(i);

		if( wayPoint->cur_unit_group.has_unit(unit_array.selected_recno) )
			break;
	}

	//--- if selected unit does not have a way point yet, add a new way point ---//

	if( i==0 )
	{
		add();								// add a new WayPoint
		wayPoint = get( size() );		// get the last WayPoint in the array
	}

	//----- add points to the WayPoint ------//

	wayPoint->add(xLoc, yLoc);
}
//----------- End of function WayPointArray::add_point ---------//


//--------- Begin of function WayPointArray::process ---------//
//
void WayPointArray::process()
{
	for( int i=size() ; i>0 ; i-- )
	{
		WayPoint* wayPoint = get(i);

		//------ process the way point now -------//

		if( !wayPoint->process() )		// if process() returns 0, delete it.
		{
			del(i);
			continue;
		}
	}
}
//----------- End of function WayPointArray::process ---------//


//------- Begin of function WayPointArray::draw -----//
//
// <int> drawMode - 1 - draw on the mini map.
//						  2 - draw on the zoom map.
//
void WayPointArray::draw(int drawMode)
{
	for( int i=size() ; i>0 ; i-- )
		get(i)->draw(drawMode);
}
//--------- End of function WayPointArray::draw ----//


//--------- Begin of function WayPointArray::add ---------//
//
// return: <int> the id. of the added WayPoint.
//
int WayPointArray::add()
{
	WayPoint* newWayPoint = new WayPoint;

	linkin(&newWayPoint);

	return recno();
}
//----------- End of function WayPointArray::add ---------//


//--------- Begin of function WayPointArray::del ---------//
//
void WayPointArray::del(int delRecno)
{
	WayPoint* wayPoint = (WayPoint*) get_ptr(delRecno);

	delete wayPoint;

	linkout(delRecno);
}
//----------- End of function WayPointArray::del ---------//


//------- Begin of function WayPointArray::get -----//

WayPoint* WayPointArray::get(int recNo)
{
	err_when( recNo < 1 || recNo > size() );

	return (WayPoint*) get_ptr(recNo);
}

//--------- End of function WayPointArray::get ----//

