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

// Filename    : OC_AU.H
// Description : header file of Campaign animation unit
//               used in Campaign::attack_animation


#ifndef __OC_AU_H
#define __OC_AU_H

// -------- define class for attack_animation -------//

class CampaignAnimationUnit
{
public:
	short unit_id;
	short	color_scheme_id;
	short	cur_x;
	short cur_y;
	short result;		// 0 not determined, 1=win, -1=lose

	// for animation
	short	scrn_x;
	short scrn_y;
	short	go_scrn_x;
	short go_scrn_y;

	void init( int unitId, int colorSchemeId, int scrnX, int scrnY, int r = 0)
	{
		unit_id = unitId;
		color_scheme_id = colorSchemeId;
		cur_x = scrnX;
		cur_y = scrnY;
		result = r;
	}
};

#endif