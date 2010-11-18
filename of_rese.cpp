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

//Filename    : OF_RESE.CPP
//Description : Firm Magic Tower

#include <OF_RESE.H>
#include <OINFO.H>
#include <OMODEID.H>
#include <ODATE.H>
#include <OSTR.H>
#include <ONEWS.H>
#include <OCONFIG.H>
#include <OSYS.H>
#include <OUNIT.H>
#include <ONATION.H>
#include <OGODRES.H>
#include <OTECHRES.H>
#include <OREMOTE.H>
#include <OSE.H>
#include <OSERES.H>


//--------- Begin of function FirmResearch::FirmResearch ---------//
//
FirmResearch::FirmResearch()
{
	needed_worker_count = MAX_WORKER;
}
//----------- End of function FirmResearch::FirmResearch -----------//


//--------- Begin of function FirmResearch::~FirmResearch ---------//
//
FirmResearch::~FirmResearch()
{
}
//----------- End of function FirmResearch::~FirmResearch -----------//


//--------- Begin of function FirmResearch::next_day ---------//
//
void FirmResearch::next_day()
{
	//----- call next_day() of the base class -----//

	FirmWork::next_day();

	//--------- make research progress ----------//

	progress_research();
}
//----------- End of function FirmResearch::next_day -----------//


//--------- Begin of function FirmResearch::progress_research --------//
//
// Process the current research.
//
void FirmResearch::progress_research()
{
	Nation* nationPtr = nation_array[nation_recno];

	if( !nationPtr->research_tech_id )
		return;

	//------- make a progress with the research ------//

	TechInfo* techInfo = tech_res[nationPtr->research_tech_id];
	float		 progressPoint;

	err_when( techInfo->get_nation_tech_level(nation_recno) >= techInfo->max_tech_level(nation_recno) );

	if( config.fast_build && nation_recno==nation_array.player_recno )
		progressPoint = (float) productivity / 100 + (float) 0.5;
	else
		progressPoint = (float) productivity / 300;

	int 	newLevel 	 = techInfo->get_nation_tech_level(nation_recno)+1;
	float levelDivider = ((float)(newLevel+1)/2);		// from 1.0 to 2.0

	progressPoint = progressPoint * (float) 30
						 / techInfo->complex_level
						 / levelDivider;					// more complex and higher level technology will take longer to research

	// ###### begin Gilbert 19/11 ########//
	// ----- effect of seat of power --------//

	if( nation_recno && god_res[GOD_INDIAN]->nation_prayer_count(nation_recno) > 0 )
	{
		progressPoint = progressPoint * 1.3f;		// increase by 30%
	}

	// ###### end Gilbert 19/11 ########//
	if( nationPtr->progress_research(progressPoint) )
	{
		if( firm_recno == firm_array.selected_recno )		// if the research is complete
			info.disp();

		// ####### begin Gilbert 26/5 #########//
		if (is_own() && info.game_date%60 == 0)
		{
			if (nationPtr->ai_research_array[0] == cast_to_Firm()->firm_recno)
				news_array.firm_idle(cast_to_Firm()->firm_recno);
		}	
		// because all tower of science research the same subject, many news message will display together ...
		// ####### end Gilbert 26/5 #########//
	}
}
//----------- End of function FirmResearch::progress_research ---------//


//--------- Begin of function FirmResearch::is_operating ---------//
//
int FirmResearch::is_operating()
{
	return worker_count && nation_array[nation_recno]->research_tech_id;
}
//----------- End of function FirmResearch::is_operating -----------//
