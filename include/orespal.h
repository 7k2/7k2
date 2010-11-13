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

//Filename    : ORESPAL.H
//Description : Palette resource


#ifndef __ORESPAL_H
#define __ORESPAL_H

#include <ORESDB.H>

class ResourcePal : public ResourceDb
{
public:
	short *pal_data_buf;

public:
   ResourcePal() : ResourceDb()   { pal_data_buf = 0; }
   ~ResourcePal()  { deinit(); }

   ResourcePal(char* resName,Database* dbObj,int indexOffset,int useCommonBuf=0)
		: ResourceDb ()
	{ init_flag=0; init(resName,dbObj,indexOffset,useCommonBuf); }

   void init(char*,Database*,int,int=0);
   void deinit();
   void  init_imported(char*,int,int=0);
	short* read_imported_pal(long);

private:
	void	generate_pal();
};

#endif