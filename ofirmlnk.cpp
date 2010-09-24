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

//Filename    : OFIRMLNK.CPP
//Description : Object Firm - functions for setting links between firms and towns 

#include <OFIRM.H>
#include <OFIRMRES.H>
#include <OREMOTE.H>
#include <OTOWN.H>
#include <OF_CAMP.H>

//------- Begin of function Firm::setup_link ---------//
//
void Firm::setup_link()
{
   //-----------------------------------------------------------------------------//
	// check the connected firms location and structure if ai_link_checked is true
	//-----------------------------------------------------------------------------//

	if(is_ai)
		ai_link_checked = 0;

	//----- build firm-to-firm link relationship -------//

	int   firmRecno, defaultLinkStatus;
	Firm* firmPtr;
	FirmInfo* firmInfo = firm_res[firm_id];

	linked_firm_count = 0;

	for( firmRecno=firm_array.size() ; firmRecno>0 ; firmRecno-- )
	{
		if( firm_array.is_deleted(firmRecno) || firmRecno==firm_recno )
			continue;

		firmPtr = firm_array[firmRecno];

		//---- do not allow links between firms of different nation ----//

		if( firmPtr->nation_recno != nation_recno && firmPtr->firm_id != FIRM_LAIR )		// A nation lair can be linked to an independent lair
			continue;

		//---------- check if the firm is close enough to this firm -------//

		if( m.points_distance( firmPtr->center_x, firmPtr->center_y,
			 center_x, center_y ) > world.effective_distance(firmPtr->firm_id, firm_id) ) 
		{
			continue;
		}

		//------ check if both are on the same terrain type ------//

		if( world.get_loc(firmPtr->loc_x1, firmPtr->loc_y1)->is_plateau()
			 != world.get_loc(loc_x1, loc_y1)->is_plateau() )
		{
			continue;
		}

		if( !firm_res.is_linkable_to_firm( firm_id, race_id, nation_recno, firmPtr->firm_id, firmPtr->race_id, firmPtr->nation_recno) )
			continue;

		//------- determine the default link status ------//

		if( firmPtr->nation_recno == nation_recno )   // if the two firms are of the same nation, get the default link status which is based on the types of the firms
			defaultLinkStatus = firmInfo->default_firm_link_status(firmPtr->firm_id);
		else
			defaultLinkStatus = LINK_DD;     // if the two firms are of different nations, default link status is both side disabled

		//-------- add the link now -------//

		if( linked_firm_count < MAX_LINKED_FIRM_FIRM )
		{
			linked_firm_array[linked_firm_count] = firmRecno;
			linked_firm_enable_array[linked_firm_count] = defaultLinkStatus;

			linked_firm_count++;
		}
		else		// we must link it as it is linked both sides, if one side is linked and the other is not, that will cause a bug
		{
			err_here();
		}

		if( firmPtr->linked_firm_count < MAX_LINKED_FIRM_FIRM )
		{
			if( defaultLinkStatus==LINK_ED )    // Reverse the link status for the opposite linker
				defaultLinkStatus=LINK_DE;

			else if( defaultLinkStatus==LINK_DE )
				defaultLinkStatus=LINK_ED;

			firmPtr->linked_firm_array[firmPtr->linked_firm_count] = firm_recno;
			firmPtr->linked_firm_enable_array[firmPtr->linked_firm_count] = defaultLinkStatus;

			firmPtr->linked_firm_count++;

			if(firmPtr->is_ai)
				firmPtr->ai_link_checked = 0;
		}
		else
		{
			err_here();
		}
	}

	//----- build firm-to-town link relationship -------//

	linked_town_count = 0;

	if( !firmInfo->is_linkable_to_town )
		return;

	int   townRecno;
	Town* townPtr;

	for( townRecno=town_array.size() ; townRecno>0 ; townRecno-- )
	{
		if( town_array.is_deleted(townRecno) )
			continue;

		townPtr = town_array[townRecno];

		//----- only linkable if the firm and the town belong to the same nation or the firm can influence a foreign town, i.e. Camp, Fort and Fryhtan Lair,

		if( townPtr->nation_recno==0 )
		{
			//--- independent towns can linked to work firms and markets ---//

			if( !cast_to_FirmWork() && !cast_to_FirmMarket() && !cast_to_FirmCamp() )
				continue;
		}
		else
		{
			//--- nation towns can only linked to own firms or camps -------//

			if( nation_recno != townPtr->nation_recno && !cast_to_FirmCamp() )
				continue;
		}

		//------ check if the town is close enough to this firm -------//

		if( m.points_distance( townPtr->center_x, townPtr->center_y,
			 center_x, center_y ) > world.effective_distance(firm_id, 0) )
		{
			continue;
		}

		//------ check if both are on the same terrain type ------//

		if( (world.get_loc(townPtr->loc_x1, townPtr->loc_y1)->is_plateau()==1)
			 != (world.get_loc(loc_x1, loc_y1)->is_plateau()==1) )
		{
			continue;
		}

		//----- if the firm requires same race -------//

		if( !firm_res.is_linkable_to_town( firm_id, race_id, townPtr->race_id) )
			continue;

		//------- determine the default link status ------//

		int defaultLinkStatus=LINK_EE;

		if( townPtr->nation_recno==0 )		// if this is an independent town
		{
			if( nation_recno==0 || townPtr->resistance(nation_recno) > INDEPENDENT_LINK_RESISTANCE )
				defaultLinkStatus = LINK_ED;
		}
/*
		//---------------------------------------------------//
		//
		// If this is a camp, it can be linked to the town when
		// either the town is an independent one or the town
		// is not linked to any camps of its own.
		//
		//---------------------------------------------------//

		if( cast_to_FirmCamp() )
		{
			if( townPtr->nation_recno==0 || !townPtr->has_linked_own_camp )
				defaultLinkStatus = LINK_EE;
		}
*/
		//-------- add the link now -------//

		if( linked_town_count < MAX_LINKED_FIRM_TOWN )
		{
			linked_town_array[linked_town_count] = townRecno;
			linked_town_enable_array[linked_town_count] = defaultLinkStatus;

			linked_town_count++;
      }
		else
		{
			err_here();
		}

		if( townPtr->linked_firm_count < MAX_LINKED_FIRM_TOWN )
      {
			if( defaultLinkStatus==LINK_ED )    // Reverse the link status for the opposite linker
				defaultLinkStatus=LINK_DE;

			else if( defaultLinkStatus==LINK_DE )
				defaultLinkStatus=LINK_ED;

			err_when( townPtr->linked_firm_count>0 && townPtr->linked_firm_array[townPtr->linked_firm_count-1] == firm_recno );		// BUGHERE check double linking error

			townPtr->linked_firm_array[townPtr->linked_firm_count] = firm_recno;
			townPtr->linked_firm_enable_array[townPtr->linked_firm_count] = defaultLinkStatus;

			townPtr->linked_firm_count++;

         if(townPtr->ai_town)
            townPtr->ai_link_checked = 0;
		}
		else
		{
			err_here();
		}
   }

	// ###### begin Gilbert 23/11 #######//
	// -------- set active link town ---------//

	if( can_set_active_link(0) && (!active_link_town_recno || !can_set_active_link(active_link_town_recno)) )	// if current active_link_town is active, don't change
	{
		short townRecnoArray[MAX_RACE];
		int raceCount = scan_best_recruitable_town(townRecnoArray);
		for( int i = 0; i < raceCount; ++i )
		{
			if( can_set_active_link(townRecnoArray[i]) )
			{
				set_active_link(townRecnoArray[i], COMMAND_AUTO);
				break;
			}
		}
	}
	// ###### end Gilbert 23/11 #######//
}
//-------- End of function Firm::setup_link -----------//


//------- Begin of function Firm::release_link ---------//
//
void Firm::release_link()
{
   int i;
   Firm *firmPtr;
   Town *townPtr;

   //------ release linked firms ------//

   for( i=0 ; i<linked_firm_count ; i++ )
   {
      firmPtr = firm_array[linked_firm_array[i]];
      firmPtr->release_firm_link(firm_recno);

      if(firmPtr->is_ai)
         firmPtr->ai_link_checked = 0;
   }

   //------ release linked towns ------//

   for( i=0 ; i<linked_town_count ; i++ )
   {
      townPtr = town_array[linked_town_array[i]];
      townPtr->release_firm_link(firm_recno);

      if(townPtr->ai_town)
			townPtr->ai_link_checked = 0;
	}

	active_link_town_recno = 0;
}
//-------- End of function Firm::release_link -----------//


//------- Begin of function Firm::release_firm_link ---------//
//
void Firm::release_firm_link(int releaseFirmRecno)
{
   //-----------------------------------------------------------------------------//
   // check the connected firms location and structure if ai_link_checked is true
   //-----------------------------------------------------------------------------//
   if(is_ai)
      ai_link_checked = 0;

   for( int i=0 ; i<linked_firm_count ; i++ )
	{
      if( linked_firm_array[i] == releaseFirmRecno )
		{
			err_when( linked_firm_count > MAX_LINKED_FIRM_FIRM );

			m.del_array_rec( linked_firm_array, linked_firm_count, sizeof(linked_firm_array[0]), i+1 );
         m.del_array_rec( linked_firm_enable_array, linked_firm_count, sizeof(linked_firm_enable_array[0]), i+1 );
         linked_firm_count--;
         return;
      }
   }

   err_here();
}
//------- End of function Firm::release_firm_link ---------//


//------- Begin of function Firm::release_town_link ---------//
//
void Firm::release_town_link(int releaseTownRecno)
{
	//-----------------------------------------------------------------------------//
   // check the connected firms location and structure if ai_link_checked is true
   //-----------------------------------------------------------------------------//

	if(is_ai)
		ai_link_checked = 0;

	for( int i=0 ; i<linked_town_count ; i++ )
   {
      if( linked_town_array[i] == releaseTownRecno )
		{
			err_when( linked_town_count > MAX_LINKED_FIRM_TOWN );

			m.del_array_rec( linked_town_array, linked_town_count, sizeof(linked_town_array[0]), i+1 );
			m.del_array_rec( linked_town_enable_array, linked_town_count, sizeof(linked_town_enable_array[0]), i+1 );
         linked_town_count--;
         return;
      }
   }

   err_here();
}
//------- End of function Firm::release_town_link ---------//



//------- Begin of function Firm::toggle_firm_link ---------//
//
// Toggle the firm link of the current firm.
//
// <int> linkId     - id. of the link
// <int> toggleFlag - 1-enable, 0-disable
// [int] setBoth - if this is 1, it will set the link to either LINK_EE or LINK_DD (and no LINK_ED or LINK_DD)
//						 if this is -1, the only one side will be set even though the nation recno of the firm and town are the same
//					    (default: 0)
//
void Firm::toggle_firm_link(int linkId, int toggleFlag, char remoteAction, int setBoth)
{
	if( !remoteAction && remote.is_enable() )
	{
		// packet structure : <firm recno> <link Id> <toggle Flag>
		short *shortPtr = (short *)remote.new_send_queue_msg(MSG_FIRM_TOGGLE_LINK_FIRM, 3*sizeof(short));
		shortPtr[0] = firm_recno;
		shortPtr[1] = linkId;
		shortPtr[2] = toggleFlag;
		return;
	}

	int linkedNationRecno = firm_array[linked_firm_array[linkId-1]]->nation_recno;

	int sameNation = linkedNationRecno == nation_recno;

//	int sameNation = linkedNationRecno == nation_recno ||    // if one of the linked end is an indepdendent firm/nation, consider this link as a single nation link
//						  linkedNationRecno == 0 ||
//                  nation_recno == 0;

	if( toggleFlag )
   {
		if( (sameNation && setBoth==0) || setBoth==1 )
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

   for( i=0 ; i<firmPtr->linked_firm_count ; i++ )
   {
      if( firmPtr->linked_firm_array[i] == firm_recno )
      {
			if( toggleFlag )
			{
				if( (sameNation && setBoth==0) || setBoth==1 )
					firmPtr->linked_firm_enable_array[i]  = LINK_EE;
				else
					firmPtr->linked_firm_enable_array[i] |= LINK_DE;
			}
			else
			{
				if( (sameNation && setBoth==0) || setBoth==1 )
					firmPtr->linked_firm_enable_array[i]  = LINK_DD;
            else
               firmPtr->linked_firm_enable_array[i] &= ~LINK_DE;
         }

			break;
      }
   }
}
//-------- End of function Firm::toggle_firm_link ---------//


//------- Begin of function Firm::toggle_town_link ---------//
//
// Toggle the town link of the current firm.
//
// <int> linkId     - id. of the link
// <int> toggleFlag - 1-enable, 0-disable
// [int] setBoth - if this is 1, it will set the link to either LINK_EE or LINK_DD (and no LINK_ED or LINK_DD)
//						 if this is -1, the only one side will be set even though the nation recno of the firm and town are the same
//					    (default: 0)
//
void Firm::toggle_town_link(int linkId, int toggleFlag, char remoteAction, int setBoth)
{
	if( !remoteAction && remote.is_enable() )
	{
		// packet structure : <firm recno> <link Id> <toggle Flag>
		short *shortPtr = (short *)remote.new_send_queue_msg(MSG_FIRM_TOGGLE_LINK_TOWN, 3*sizeof(short));
		shortPtr[0] = firm_recno;
		shortPtr[1] = linkId;
		shortPtr[2] = toggleFlag;
		return;
	}

   int linkedNationRecno = town_array[linked_town_array[linkId-1]]->nation_recno;

	int sameNation = linkedNationRecno == nation_recno;

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

	for( i=0 ; i<townPtr->linked_firm_count ; i++ )
	{
		if( townPtr->linked_firm_array[i] == firm_recno )
		{
			if( toggleFlag )
			{
				if( (sameNation && setBoth==0) || setBoth==1 )
					townPtr->linked_firm_enable_array[i]  = LINK_EE;
				else
					townPtr->linked_firm_enable_array[i] |= LINK_DE;
			}
			else
			{
				if( (sameNation && setBoth==0) || setBoth==1 )
               townPtr->linked_firm_enable_array[i]  = LINK_DD;
            else
               townPtr->linked_firm_enable_array[i] &= ~LINK_DE;
         }

			break;
		}
	}

	//-------- update the town's influence --------//

	if( townPtr->nation_recno==0 )
		townPtr->update_target_resistance();

	//--- redistribute demand if a link to market place has been toggled ---//

	if( firm_id == FIRM_MARKET )
		town_array.distribute_demand();
}
//-------- End of function Firm::toggle_town_link ---------//


// ------ Begin of function Firm::get_linked_firm_status ------//
//
// get the link status to another firm
// return LINK_DD if not connected
//
char Firm::get_linked_firm_status(int firmRecno)
{
	for( int i = linked_firm_count-1; i >= 0; --i )
	{
		if( linked_firm_array[i] == firmRecno )
			return linked_firm_enable_array[i];
	}
	return (char) LINK_DD;
}
// ------ end of function Firm::get_linked_firm_status ------//



// ------ Begin of function Firm::get_linked_town_status ------//
//
// get the link status to another town
// return LINK_DD if not connected
//
char Firm::get_linked_town_status(int townRecno)
{
	for( int i = linked_town_count-1; i >= 0; --i )
	{
		if( linked_town_array[i] == townRecno )
			return linked_town_enable_array[i];
	}
	return (char) LINK_DD;
}
// ------ end of function Firm::get_linked_town_status ------//


//------- Begin of function Firm::scan_best_recruitable_town ---------//
//
// scan best towns for recruitment
//
// <short *>townRecnoArray   array of townRecnoArray to hold town recnos. The array size is MAX_RACE.
// [int] raceId              race_id required of the towns, 0 if default of firm (default:0)
// [int] requiredLinkedFirmIed  town consider if that town has enabled firm linked (default:0)
// return : number of suitable town
//
int Firm::scan_best_recruitable_town(short *townRecnoArray, int raceId, int requiredLinkedFirmId)
{
	// ------ scan most suitable town for each race -----//

	short mostSuitableTown[MAX_RACE];
	memset( mostSuitableTown, 0, sizeof(mostSuitableTown) );
	long valueTown[MAX_RACE];
	memset( valueTown, 0, sizeof(valueTown) );

	//-------- adjust raceId ---------//

	for( int i = linked_town_count-1; i >= 0; --i )
	{
		err_when( town_array.is_deleted(linked_town_array[i]));

		Town *townPtr = town_array[linked_town_array[i]];

		if( !can_train_from_town(townPtr->town_recno, requiredLinkedFirmId) )
			continue;

		long value = ((long)townPtr->loyalty - MIN_RECRUIT_LOYALTY);

		if( value > valueTown[townPtr->race_id-1] )
		{
			mostSuitableTown[townPtr->race_id-1] = linked_town_array[i];
			valueTown[townPtr->race_id-1] = value;
		}
	}

	//------------------------------------//

	long raceCount = 0;

	for( i = 0; i < MAX_RACE; ++i )
	{
		if( mostSuitableTown[i] )
		{
			if( townRecnoArray )
			{
				*townRecnoArray = mostSuitableTown[i];
				++townRecnoArray;
			}
			++raceCount;
		}
	}

	return raceCount;
}
//------- End of function Firm::scan_best_recruitable_town ---------//


//------- Begin of function Firm::can_train_from_town ---------//
//
// Return whether we can recruit a specific unit from the specific town.
//
// <int> townRecno            - the town where the new unit will be trained
// <int> requiredLinkedFirmId - the id. of the required firm for training a unit
// [int] raceId					- the race id of the unit to be trained
//										  (default: 0 - any race)
//
// return: 1 succeed or 0 failed
//
int Firm::can_train_from_town(int townRecno, int requiredLinkedFirmId, int raceId)
{
	Town* townPtr = town_array[townRecno];

	if( !raceId && firm_res[firm_id]->same_race_worker )
		raceId = race_id;

	if( townPtr->nation_recno != nation_recno
		|| ( raceId && townPtr->race_id != raceId )
		|| townPtr->loyalty < MIN_RECRUIT_LOYALTY
		|| townPtr->recruitable_pop(1) <= 0 )
	{
		return 0;
	}

	if( !requiredLinkedFirmId )		// if the training does not required a linked firm, return true now
		return 1;

	//----- check if town has linked specific firm ------//

	for( int i = townPtr->linked_firm_count-1; i >= 0; --i )
	{
		err_when( firm_array.is_deleted(townPtr->linked_firm_array[i]) );
		Firm *firmPtr = firm_array[townPtr->linked_firm_array[i]];

		if( firmPtr->firm_id == requiredLinkedFirmId
			&& firmPtr->nation_recno == townPtr->nation_recno
			&& (!firm_res[firmPtr->firm_id]->same_race_worker || firmPtr->race_id == townPtr->race_id )
			&& townPtr->linked_firm_enable_array[i] == LINK_EE
			&& !firmPtr->under_construction )
		{
			return 1;
		}
	}

	return 0;
}
//------- End of function Firm::can_train_from_town ---------//


//------- Begin of function Firm::set_active_link ---------//
//
void Firm::set_active_link(int townRecno, char remoteAction)
{
	if( !remoteAction && remote.is_enable() )
	{
		// packet structure : <firm recno> <town recno>
		short *shortPtr = (short *)remote.new_send_queue_msg( MSG_FIRM_SET_ACTIVE_LINK, 2*sizeof(short) );
		shortPtr[0] = firm_recno;
		shortPtr[1] = townRecno;
		return;
	}
}
//------- End of function Firm::set_active_link ---------//
