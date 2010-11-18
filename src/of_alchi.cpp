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

// Filename    : OF_ALCHI.CPP
// Description : Alchemy Tor interface


#include <of_alch.h>
#include <ovga.h>
#include <omodeid.h>
#include <omouse.h>
#include <ofont.h>
#include <oimgres.h>
#include <obutt3d.h>
#include <oremote.h>
#include <oinfo.h>
#include <orawres.h>
#include <otechres.h>
#include <otech.h>
#include <ot_firm.h>


// -------- define constant --------//

#define MAX_CONVERSION 5

// -------- defind static variable ---------//

static Button3D	button_change;


void FirmMonsterAlchemy::put_info(int refreshFlag)
{
	Firm::put_info(refreshFlag);

	if( !should_show_info() )
		return;
	
	vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("MISSBASE") );

	disp_firm_info(INFO_Y1, refreshFlag);
}

void FirmMonsterAlchemy::detect_info()
{
	Firm::detect_info();

	detect_firm_info();
}

void FirmMonsterAlchemy::disp_firm_info(int dispY1, int refreshFlag )
{
	if( refreshFlag == INFO_REPAINT )
	{
		button_change.create( INFO_X1+13, INFO_Y1+235, 'A', "CHG-TASK" );
	}

	if( should_show_info() )
	{
//		int x2;

		// -------- display material --------//

		if( raw_id )
		{
//			String str = raw_res[raw_id]->name;
//			str += " Deposit";
			font_whbl.center_put( INFO_X1+20, INFO_Y1+20, INFO_X2-10, INFO_Y1+40, 
				text_firm.str_alchemy_raw(raw_id) );

			// str = "Estimated Reserves ";
			// str += m.format((int)reserve_qty, 1);
			font_whbl.center_put( INFO_X1+20, INFO_Y1+40, INFO_X2-10, INFO_Y1+60, 
				text_firm.str_estimated_resource((int)reserve_qty) );

			vga.active_buf->put_bitmap_trans( INFO_X1+13, INFO_Y1+129, raw_res.interface_raw_icon(raw_id) );
		}

		// -------- display tech id ---------- //

		if( !conversion_tech_id )
		{
			font_whbl.center_put( INFO_X1+20, INFO_Y1+100, INFO_X2-10, INFO_Y1+120, 
				text_firm.str_alchemy_idle() ); // "No conversion in progress" );
		}
		else
		{
			String str;
			// str = "Converting ";
			// str += tech_res[conversion_tech_id]->tech_des();
			font_whbl.center_put( INFO_X1+20, INFO_Y1+100, INFO_X2-10, INFO_Y1+120, 
				text_firm.str_conversion(tech_res[conversion_tech_id]->tech_des()) );

			// ------- display productivity --------- //

			// str = "Monthly Production: ";
			str = text_firm.str_monthly_production();
			str += ": ";
			str += (int) production_30days();
			font_whbl.center_put( INFO_X1+20, INFO_Y1+120, INFO_X2-10, INFO_Y1+140, str );

		//	x2 = font_snds.put( INFO_X1+20, INFO_Y1+118, "Monthly Production: ");
		//	x2 = font_snds.put( x2, INFO_Y1+118, (int) production_30days() );
		}
	}

	if( is_own() )
	{
		button_change.paint();
	}
}



void FirmMonsterAlchemy::detect_firm_info()
{
	if( is_own() )
	{
		if( button_change.detect() )
		{
			// find next newTechId

			int newTechId = conversion_tech_id;	
			int t = MAX_CONVERSION;
			do
			{
				switch(newTechId)
				{
				case 0:                  newTechId = TECH_LIVE_TO_GOLD;   break;
				case TECH_LIVE_TO_GOLD:  newTechId = TECH_GOLD_TO_LIVE;   break;
				case TECH_GOLD_TO_LIVE:  newTechId = TECH_RAW_TO_LIVE;    break;
				case TECH_RAW_TO_LIVE:   newTechId = TECH_RAW_TO_GOLD;    break;
				case TECH_RAW_TO_GOLD:   newTechId = 0;                   break;
				default: err_here();
				}
				--t;
			} while( newTechId && !can_converse(newTechId) && t > 0);
			if( !can_converse(newTechId) )
				newTechId = 0;

			change_conversion(newTechId, COMMAND_PLAYER );
		}
	}
}
