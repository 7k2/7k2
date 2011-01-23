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

// Filename     : OCNATION.CPP
// Description  : CampaignNation functions

#include <ostr.h>
#include <ogame.h>
#include <ocampgn.h>
#include <onation.h>
#include <ogameset.h>
#include <oraceres.h>
#include <omonsres.h>
#include <ot_unit.h>

//-------- Begin of function CampaignNation::init ------//

void CampaignNation::init()
{
	memset(this, 0, sizeof(*this));
}
//--------- End of function CampaignNation::init -------//


//-------- Begin of function CampaignNation::deinit ------//

void CampaignNation::deinit()
{
	campaign_nation_recno = 0;			// mark deleted
}
//-------- End of function CampaignNation::deinit ------//


//-------- Begin of function CampaignNation::init_nation ------//

void CampaignNation::init_nation(int nationType, int raceId, int colorSchemeId)
{
	nation_type = nationType;
	race_id = raceId;
	color_scheme_id = colorSchemeId;
	nation_color	 = game.color_remap_array[colorSchemeId].main_color;

	noble_count = 0;

	//---- set king's name and custom name in CampaignNation ----//

	String str;

	if( is_human() )
	{
		king_name_id = race_res[race_id]->get_new_name_id();

//		str  = race_res[race_id]->get_name(king_name_id, 1);	// 1-first name only
//		str += "'s Kingdom";	
		str = text_unit.str_unit_nation( race_id, race_res[race_id]->get_name(king_name_id, 1) );	// 1-first name only
	}
	else
	{
		king_name_id = monster_res.get_new_name_id();

//		str  = monster_res.get_name(king_name_id);
//		str += "'s Kwyzan";
		str = text_unit.str_unit_nation( race_id, monster_res.get_name(king_name_id) );
	}

	set_custom_name(str);

	//---- initialize names of the ministers ----//

	for( int i=0 ; i<MINISTER_COUNT ; i++ )
	{
		if( is_human() )
			minister_name_array[i] = race_res[race_id]->get_new_name_id();
		else
			minister_name_array[i] = monster_res.get_new_name_id();
	}
}
//-------- End of function CampaignNation::init_nation ------//


//-------- Begin of function CampaignNation::set_custom_name ------//

void CampaignNation::set_custom_name(char* customName)
{
	strncpy( custom_name, customName, CUSTOM_NAME_LEN );

	custom_name[CUSTOM_NAME_LEN] = NULL;
}
//-------- End of function CampaignNation::set_custom_name ------//


//-------- Begin of function CampaignNation::race_name ------//

char* CampaignNation::race_name()
{
	if( is_monster() )
		return monster_res[monster_id()]->name;
	else
		return race_res[race_id]->name;
}
//-------- End of function CampaignNation::race_name ------//


//-------- Begin of function CampaignNation::king_name ------//

const char* CampaignNation::king_name()
{
	if( is_monster() )
		return monster_res.get_name(king_name_id);
	else
		return race_res[race_id]->get_name(king_name_id);
}
//-------- End of function CampaignNation::king_name ------//


//-------- Begin of function CampaignNation::minister_name ------//

const char* CampaignNation::minister_name(int ministerId)
{
	int ministerNameId = minister_name_array[ministerId-1];

	if( is_monster() )
		return monster_res.get_name(ministerNameId);
	else
		return race_res[race_id]->get_name(ministerNameId);
}
//-------- End of function CampaignNation::minister_name ------//

