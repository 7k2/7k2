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

// Filename    : i_effect.cpp
// Description : 16-bit color effect

#include <imgfun.h>

uint32_t effect_proc( int effect_id, uint16_t dest_pixel )
{
	const int effect = effect_id - MIN_EFFECT_CODE;
	const uint32_t mask = 0x7BEF;
	uint32_t result = 0;
	switch (effect)
	{
	case 0:  // dim filter
		result = (dest_pixel >> 1) & mask;
		break;
	case 1:  // dim filter
		result = (dest_pixel >> 1) & mask;
		break;
	case 2:  // blue filter
		result = ((dest_pixel >> 1) & mask) + 0x10;
		break;
	case 3:  // green filter
		result = ((dest_pixel >> 1) & mask) + 0x400;
		break;
	case 4:  // cyan filter
		result = ((dest_pixel >> 1) & mask) + 0x410;
		break;
	case 5:  // red filter
		result = ((dest_pixel >> 1) & mask) + 0x8000;
		break;
	case 6:  // purple filter
		result = ((dest_pixel >> 1) & mask) + 0x8010;
		break;
	case 7:  // yellow filter
		result = ((dest_pixel >> 1) & mask) + 0x8400;
		break;
	case 8:  // white filter
		result = ((dest_pixel >> 1) & mask) + 0x8410;
		break;
	default:
		// Error.
		break;
	}
	return result;
}
