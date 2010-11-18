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

//Filename    : OSITE.H
//Description : Object Site 

#ifndef __OSITE_H
#define __OSITE_H

#ifndef __ODYNARRB_H
#include <odynarrb.h>
#endif

#ifndef __ORAWRES_H
#include <orawres.h>
#endif

//-------- Define Site Type --------//

enum { MAX_SITE_TYPE=5 };

enum { SITE_RAW=1,
		 SITE_SCROLL,
		 SITE_GOLD_COIN,
		 SITE_ITEM,
		 SITE_WEAPON_BLUEPRINT,
};

//--------- Define class Site ----------//

class Site
{
public:
	short site_recno;

	char  site_type;		// SITE_RAW, SITE_ARTIFACT or SITE_SCROLL

	short object_id;		// id. of the object,
	int   reserve_qty;	// for raw material only
	char  has_mine;		// whether there is a mine on this site

	short map_x_loc;
	short map_y_loc;
	short map_x2;
	short map_y2;
	short	altitude;

	BYTE	region_id;
	short  animation_count;

public:
	void 	init(int siteRecno, int siteType, int xLoc, int yLoc);
	void 	deinit();

	void 	disp_info(int refreshFlag);
	void 	detect_info();

	void  draw(int curX, int curY, int curZ, int drawFlag);
	void	draw_selected();
	void	disp_edit_mode(int refreshFlag);
	int	detect_edit_mode();

	int   get_site_object(int unitRecno);
	int  	ai_get_site_object();

	// ------ visibility function -------//

	int	is_stealth();

	//------- multiplayer checking codes -------//

	UCHAR crc8();
	void	clear_ptr();
};

//--------- Define class SiteArray ----------//

class SiteArray : public DynArrayB
{
public:
	short	 selected_recno;			// the firm current being selected
	short	 untapped_raw_count;		// no. of unoccupied raw site available
	short	 scroll_count;
	short	 gold_coin_count;
	short  item_count;
	short  weapon_blueprint_count;
	short	 std_raw_site_count;		// standard no. of raw site in one game, based on this number, new mines pop up when existing mines run out of deposit
	
public:
	SiteArray();
	~SiteArray();

	void	init();
	void 	deinit();

	int 	add_site(int xLoc, int yLoc, int siteType, int objectId, int reserveQty=0);
	void 	del_site(int siteRecno);

	void	generate_raw_site(int stdRawSiteCount=0);
	int  	create_raw_site(int regionId, int townRecno=0);

	int 	scan_site(int xLoc, int yLoc, int siteType=0);
	void 	go_to_a_raw_site();
	void 	ai_get_site_object();

   void	next_day();
	void	draw_dot();

	int 	write_file(File* filePtr);
	int	read_file(File* filePtr);

	//--------------------------------------//

	int  is_deleted(int recNo);

	#ifdef DEBUG
		Site* operator()();            // reference to current Site record
		Site* operator[](int recNo);
	#else
		Site* operator()()  	   	 { return (Site*) get(); }
		Site* operator[](int recNo)  { return (Site*) get(recNo); }
	#endif
};

extern SiteArray site_array;

//--------------------------------------------//

#endif