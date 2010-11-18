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

//Filename    : OATTACK.H
//Description : Header file of Attack Attribute

#ifndef __OATTACK_H
#define __OATTACK_H

//-------- Define struct DefenseAttribute -------//

struct DefenseAttribute
{
	char	 sturdiness;		
	char	 is_wood;

	void	init(char s, char w) { sturdiness = s;
											is_wood = w;	}
};

//-------- Define struct AttackAttribute -------//

struct AttackAttribute
{
	char	 sturdiness;		
	char	 explosiveness;		
	char	 heat;
	char	 wood_favour;

	void	init(char s, char e, char h, char w) { sturdiness = s;
																explosiveness = e;
																heat = h;
																wood_favour = w; }

	float	 effectiveness ( DefenseAttribute & );
};

#endif
