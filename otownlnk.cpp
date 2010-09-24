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

//Filename   : OTOWNLNK.CPP
//Description: class Town - functions for setting up and updating links

#include <ALL.H>
#include <OTOWN.H>
#include <ONATION.H>
#include <OFIRM.H>
#include <OF_CAMP.H>
#include <OREMOTE.H>

//------- Begin of function Town::setup_link ---------//
//
void Town::setup_link()
{
	//-----------------------------------------------------------------------------//
	// check the connected firms location and structure if ai_link_checked is true
	//-----------------------------------------------------------------------------//

	if(ai_town)
		ai_link_checked = 0;

	//----- build town-to-firm link relationship -------//

	int   firmRecno;
	Firm* firmPtr;
	FirmInfo* firmInfo;

	linked_firm_count = 0;

	for( firmRecno=firm_array.size() ; firmRecno>0 ; firmRecno-- )
	{
		if( firm_array.is_deleted(firmRecno) )
			continue;

		firmPtr  = firm_array[firmRecno];
		firmInfo = firm_res[firmPtr->firm_id];

		if( !firmInfo->is_linkable_to_town )
			continue;

		//----- only linkable if the firm and the town belong to the same nation or the firm can influence a foreign town, i.e. Camp, Fort and Fryhtan Lair,

		if( nation_recno==0 )
		{
			//--- independent towns can linked to work firms and markets ---//

			if( !firmPtr->cast_to_FirmWork() && !firmPtr->cast_to_FirmMarket()
				 && !firmPtr->cast_to_FirmCamp() )
			{
				continue;
			}
		}
		else
		{
			//--- nation towns can only linked to own firms or camps -------//

			if( firmPtr->nation_recno != nation_recno && !firmPtr->cast_to_FirmCamp() )
				continue;
		}

		//---------- check if the firm is close enough to this firm -------//

		if( m.points_distance( firmPtr->center_x, firmPtr->center_y, center_x, center_y )
			 > world.effective_distance(firmPtr->firm_id, 0) )
		{
			continue;
		}

		//------ check if both are on the same terrain type ------//

		if( (world.get_loc(firmPtr->loc_x1, firmPtr->loc_y1)->is_plateau()==1)
			 != (world.get_loc(loc_x1, loc_y1)->is_plateau()==1) )
		{
			continue;
		}

		//----- if the firm requires same race -------//

		if( !firm_res.is_linkable_to_town(firmPtr->firm_id, firmPtr->race_id, race_id) )
			continue;

		//------- determine default link flag -------//

		int defaultLinkStatus = LINK_EE;

		if( nation_recno==0 )		// if this is an independent town
		{
			if( firmPtr->nation_recno==0 || resistance(firmPtr->nation_recno) > INDEPENDENT_LINK_RESISTANCE )
				defaultLinkStatus = LINK_DD;
		}

		//-------- add the link now -------//

		if( linked_firm_count < MAX_LINKED_FIRM_TOWN )
		{
			err_when( linked_firm_count>0 && linked_firm_array[linked_firm_count-1] == firmRecno );		// BUGHERE - check double linking error

			linked_firm_array[linked_firm_count] = firmRecno;
			linked_firm_enable_array[linked_firm_count] = defaultLinkStatus;

			linked_firm_count++;
		}
		else
		{
			err_here();
		}

		if( firmPtr->linked_town_count < MAX_LINKED_TOWN_TOWN )
		{
			firmPtr->linked_town_array[firmPtr->linked_town_count] = town_recno;
			firmPtr->linked_town_enable_array[firmPtr->linked_town_count] = defaultLinkStatus;

			firmPtr->linked_town_count++;

			if(firmPtr->is_ai)
				firmPtr->ai_link_checked = 0;
		}
		else
		{
			err_here();
		}
	}

	//----- build town-to-town link relationship -------//

	linked_town_count = 0;

	int   townRecno;
	Town* townPtr;

	for( townRecno=town_array.size() ; townRecno>0 ; townRecno-- )
	{
		if( town_array.is_deleted(townRecno) || townRecno==town_recno )
			continue;

		townPtr = town_array[townRecno];

		//----- if the firm requires same race -------//

		if( race_id != townPtr->race_id )
			continue;

		//------ check if the town is close enough to this firm -------//

		if( m.points_distance( townPtr->center_x, townPtr->center_y,
			 center_x, center_y ) > world.effective_distance(0, 0) )
		{
			continue;
		}

		//------ check if both are on the same terrain type ------//

		if( (world.get_loc(townPtr->loc_x1, townPtr->loc_y1)->is_plateau()==1)
			 != (world.get_loc(loc_x1, loc_y1)->is_plateau()==1) )
		{
			continue;
		}

		//-------- link this town -------//

		if( linked_town_count < MAX_LINKED_TOWN_TOWN )
		{
			linked_town_array[linked_town_count] = townRecno;
			linked_town_enable_array[linked_town_count] = LINK_EE;

			linked_town_count++;
		}
		else
		{
			err_here();
		}

		//-------- link the other town  -------//

		if( townPtr->linked_town_count < MAX_LINKED_TOWN_TOWN )
		{
			townPtr->linked_town_array[townPtr->linked_town_count] = town_recno;
			townPtr->linked_town_enable_array[townPtr->linked_town_count] = LINK_EE;

			townPtr->linked_town_count++;

			if(townPtr->ai_town)
				townPtr->ai_link_checked = 0;
		}
		else
		{
			err_here();
		}
	}
}
//-------- End of function Town::setup_link -----------//


//------- Begin of function Town::release_link ---------//
//
void Town::release_link()
{
   int i;
	Firm *firmPtr;
	Town *townPtr;

   //------ release linked firms ------//

	for( i=0 ; i<linked_firm_count ; i++ )
	{
		firmPtr = firm_array[linked_firm_array[i]];
		firmPtr->release_town_link(town_recno);
		
		if(firmPtr->is_ai)
			firmPtr->ai_link_checked = 0;

		if( firmPtr->active_link_town_recno == town_recno )
			firmPtr->active_link_town_recno = 0;
   }

   //------ release linked towns ------//

   for( i=0 ; i<linked_town_count ; i++ )
   {
		townPtr = town_array[linked_town_array[i]];
		townPtr->release_town_link(town_recno);
		
		if(townPtr->ai_town)
			townPtr->ai_link_checked = 0;
	}
}
//-------- End of function Town::release_link -----------//


//------- Begin of function Town::release_firm_link ---------//
//
void Town::release_firm_link(int releaseFirmRecno)
{
	//-----------------------------------------------------------------------------//
	// check the connected firms location and structure if ai_link_checked is true
	//-----------------------------------------------------------------------------//
	if(ai_town)
		ai_link_checked = 0;

	for( int i=0 ; i<linked_firm_count ; i++ )
   {
      if( linked_firm_array[i] == releaseFirmRecno )
      {
			err_when( linked_firm_count > MAX_LINKED_FIRM_TOWN );

			m.del_array_rec( linked_firm_array		  , linked_firm_count, sizeof(linked_firm_array[0]), i+1 );
			m.del_array_rec( linked_firm_enable_array, linked_firm_count, sizeof(linked_firm_enable_array[0]), i+1 );
			linked_firm_count--;
			return;
		}
	}

	err_here();
}
//------- End of function Town::release_firm_link ---------//


//------- Begin of function Town::release_town_link ---------//
//
void Town::release_town_link(int releaseTownRecno)
{
	//-----------------------------------------------------------------------------//
	// check the connected firms location and structure if ai_link_checked is true
	//-----------------------------------------------------------------------------//
	if(ai_town)
		ai_link_checked = 0;

	for( int i=0 ; i<linked_town_count ; i++ )
	{
		if( linked_town_array[i] == releaseTownRecno )
		{
			err_when( linked_town_count > MAX_LINKED_TOWN_TOWN );

			m.del_array_rec( linked_town_array		  , linked_town_count, sizeof(linked_town_array[0]), i+1 );
			m.del_array_rec( linked_town_enable_array, linked_town_count, sizeof(linked_town_enable_array[0]), i+1 );
			linked_town_count--;
			return;
		}
	}

	err_here();
}
//------- End of function Town::release_town_link ---------//


//------- Begin of function Town::toggle_firm_link ---------//
//
// Toggle the firm link of the current firm.
//
// <int> linkId - id. of the link
// <int> toggleFlag - 1-enable, 0-disable
// <char> remoteAction - remote action type 
// [int] setBoth - if this is 1, it will set the link to either LINK_EE or LINK_DD (and no LINK_ED or LINK_DD)
//						 if this is -1, the only one side will be set even though the nation recno of the firm and town are the same
//					    (default: 0)
//
void Town::toggle_firm_link(int linkId, int toggleFlag, char remoteAction, int setBoth)
{
	if( !remoteAction && remote.is_enable() )
	{
		// packet structure : <town recno> <link Id> <toggle Flag>
		short *shortPtr = (short *)remote.new_send_queue_msg(MSG_TOWN_TOGGLE_LINK_FIRM, 3*sizeof(short));
		shortPtr[0] = town_recno;
		shortPtr[1] = linkId;
		shortPtr[2] = toggleFlag;
		return;
	}

	Firm* linkedFirm = firm_array[linked_firm_array[linkId-1]];
	int 	linkedNationRecno = linkedFirm->nation_recno;

	int sameNation = linkedNationRecno == nation_recno;

//	int sameNation = linkedNationRecno == nation_recno ||		// if one of the linked end is an indepdendent firm/nation, consider this link as a single nation link
//						  linkedNationRecno == 0 ||
//						  nation_recno == 0;

	if( toggleFlag )
	{
		if( (sameNation && setBoth==0) || setBoth==1 )		// 0 if setBoth == -1
			linked_firm_enable_array[linkId-1] = LINK_EE;
		else
			linked_firm_enable_array[linkId-1] |= LINK_ED;
	}
	else
	{
		if( (sameNation && setBoth==0) || setBoth==1 )
			linked_firm_enable_array[linkId-1] = LINK_DD;
		else
			linked_firm_enable_array[linkId-1] &= ~LINK_ED;
	}

	//------ set the linked flag of the opposite firm -----//

	Firm* firmPtr = firm_array[ linked_firm_array[linkId-1] ];
	int   i;

	for(i=firmPtr->linked_town_count-1; i>=0; i--)
	{
		if( firmPtr->linked_town_array[i] == town_recno )
		{
			if( toggleFlag )
			{
				if( (sameNation && setBoth==0) || setBoth==1 )
					firmPtr->linked_town_enable_array[i]  = LINK_EE;
				else
					firmPtr->linked_town_enable_array[i] |= LINK_DE;
			}
			else
			{
				if( (sameNation && setBoth==0) || setBoth==1 )
					firmPtr->linked_town_enable_array[i]  = LINK_DD;
				else
					firmPtr->linked_town_enable_array[i] &= ~LINK_DE;
			}

			break;
		}
	}

	//-------- update the town's influence --------//

	if( nation_recno==0 )
		update_target_resistance();

	//--- redistribute demand if a link to market place has been toggled ---//

	if( linkedFirm->firm_id == FIRM_MARKET )
		town_array.distribute_demand();
}
//-------- End of function Town::toggle_firm_link ---------//


//------- Begin of function Town::toggle_town_link ---------//
//
// Toggle the town link of the current firm.
//
// <int> linkId - id. of the link
// <int> toggleFlag - 1-enable, 0-disable
// [int] setBoth - if this is 1, it will set the link to either LINK_EE or LINK_DD (and no LINK_ED or LINK_DD)
//						 if this is -1, the only one side will be set even though the nation recno of the firm and town are the same
//					    (default: 0)
//
void Town::toggle_town_link(int linkId, int toggleFlag, char remoteAction, int setBoth)
{
	if( !remoteAction && remote.is_enable() )
	{
		// packet structure : <town recno> <link Id> <toggle Flag>
		short *shortPtr = (short *)remote.new_send_queue_msg(MSG_TOWN_TOGGLE_LINK_TOWN, 3*sizeof(short));
		shortPtr[0] = town_recno;
		shortPtr[1] = linkId;
		shortPtr[2] = toggleFlag;
		return;
	}

	int linkedNationRecno = town_array[linked_town_array[linkId-1]]->nation_recno == nation_recno;

	int sameNation = linkedNationRecno == nation_recno;

//	int sameNation = linkedNationRecno == nation_recno ||		// if one of the linked end is an indepdendent firm/nation, consider this link as a single nation link
//						  linkedNationRecno == 0 ||
//						  nation_recno == 0;

	if( toggleFlag )
	{
		if( (sameNation && setBoth==0) || setBoth==1 )
			linked_town_enable_array[linkId-1]  = LINK_EE;
		else
			linked_town_enable_array[linkId-1] |= LINK_ED;
	}
	else
	{
		if( (sameNation && setBoth==0) || setBoth==1 )
			linked_town_enable_array[linkId-1]  = LINK_DD;
		else
			linked_town_enable_array[linkId-1] &= ~LINK_ED;
	}

	//------ set the linked flag of the opposite town -----//

	Town* townPtr = town_array[ linked_town_array[linkId-1] ];
	int   i;

	for(i=townPtr->linked_town_count-1; i>=0; i--)
	{
		if( townPtr->linked_town_array[i] == town_recno )
		{
			if( toggleFlag )
			{
				if( (sameNation && setBoth==0) || setBoth==1 )
					townPtr->linked_town_enable_array[i]  = LINK_EE;
				else
					townPtr->linked_town_enable_array[i] |= LINK_DE;
			}
			else
			{
				if( (sameNation && setBoth==0) || setBoth==1 )
					townPtr->linked_town_enable_array[i]  = LINK_DD;
				else
					townPtr->linked_town_enable_array[i] &= ~LINK_DE;
			}

			break;
		}
	}
}
//-------- End of function Town::toggle_town_link ---------//


//-------- Begin of function Town::linked_camp_count ------//
//
// Return the number of linked military camp to this town.
//
// <bool> hasOverseer - only count the camps with an overseer.
//
int Town::linked_camp_count(bool hasOverseer)
{
	int 	i, linkedCount=0;
	Firm* firmPtr;

	for( i=0 ; i<linked_firm_count ; i++ )
	{
		if( linked_firm_enable_array[i] == LINK_EE )
		{
			firmPtr = firm_array[linked_firm_array[i]];

			if( firmPtr->cast_to_FirmCamp() )
			{
				if( !hasOverseer || firmPtr->cast_to_FirmCamp()->overseer_recno )
				{
					linkedCount++;
				}
			}
		}
	}

	return linkedCount;
}
//---------- End of function Town::linked_camp_count --------//


//----- Begin of function Town::has_linked_camp -----//
//
// Return whether there is a camp of the specific nation
// linked to this town.
//
// <int> nationRecno  - recno of the nation.
// <int> needOverseer - whether only count camps with overseers or not.
//
int Town::has_linked_camp(int nationRecno, int needOverseer)
{
	Firm* firmPtr;

	for( int i=0 ; i<linked_firm_count ; i++ )
	{
		firmPtr = firm_array[ linked_firm_array[i] ];
	
		if( firmPtr->cast_to_FirmCamp() &&
			 firmPtr->nation_recno == nationRecno )
		{
			if( !needOverseer || firmPtr->cast_to_FirmCamp()->overseer_recno )
				return 1;
		}
	}

	return 0;
}
//------ End of function Town::has_linked_camp -----//


//----- Begin of function Town::update_camp_link -----//
//
// Update the status of links from this town to camps.
//
void Town::update_camp_link()
{
	//--- enable the link of the town's side to all linked camps ---//

	Firm* firmPtr;

	for( int i=0 ; i<linked_firm_count ; i++ )
	{
		 firmPtr = firm_array[linked_firm_array[i]];

		 if( !firmPtr->cast_to_FirmCamp() )	 
			 continue;

		 //--- don't set it if the town and camp both belong to a human player, the player will set it himself ---//

		 if( firmPtr->nation_recno == nation_recno &&
			  nation_recno && !nation_array[nation_recno]->is_ai() )
		 {
			 continue;
		 }

		 //--------------------------------------------//

		 toggle_firm_link( i+1, 1, COMMAND_AUTO );
	}

	//------- update camp link status -------//

	has_linked_own_camp = 0;
	has_linked_enemy_camp = 0;

	for( i=0 ; i<linked_firm_count ; i++ )
	{
		if( linked_firm_enable_array[i] != LINK_EE )
			continue;

		firmPtr = firm_array[linked_firm_array[i]];

		if( !firmPtr->cast_to_FirmCamp() || 
			firmPtr->cast_to_FirmCamp()->overseer_recno == 0 )
		{
			continue;
		}

		if( firmPtr->nation_recno == nation_recno )
			has_linked_own_camp = 1;
		else
			has_linked_enemy_camp = 1;
	}
}
//------ End of function Town::update_camp_link -----//



// ###### begin Gilbert 3/11 #######//
// ------ Begin of function Town::get_linked_firm_status ------//
//
// get the link status to another firm
// return LINK_DD if not connected
//
char Town::get_linked_firm_status(int firmRecno)
{
	for( int i = linked_firm_count-1; i >= 0; --i )
	{
		if( linked_firm_array[i] == firmRecno )
			return linked_firm_enable_array[i];
	}
	return (char) LINK_DD;
}
// ------ end of function Town::get_linked_firm_status ------//



// ------ Begin of function town::get_linked_town_status ------//
//
// get the link status to another town
// return LINK_DD if not connected
//
char Town::get_linked_town_status(int townRecno)
{
	for( int i = linked_town_count-1; i >= 0; --i )
	{
		if( linked_town_array[i] == townRecno )
			return linked_town_enable_array[i];
	}
	return (char) LINK_DD;
}
// ------ end of function Town::get_linked_town_status ------//

// ###### end Gilbert 3/11 #######//

