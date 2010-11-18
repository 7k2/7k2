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

//Filename    : OR_NEWS.CPP
//Description : Report for displaying AI actions

#include <ovga.h>
#include <omodeid.h>
#include <odate.h>
#include <osys.h>
#include <ofont.h>
#include <omouse.h>
#include <oimgres.h>
#include <ovbrowif.h>
#include <obutton.h>
#include <otalkres.h>
#include <onews.h>
#include <oinfo.h>
// #include <oworldmt.h>
 
//------------- Define coordinations -----------//

//enum { NEWS_BROWSE_X1 = ZOOM_X1+6,
//		 NEWS_BROWSE_Y1 = ZOOM_Y1+6,
//		 NEWS_BROWSE_X2 = ZOOM_X2-6,
//		 NEWS_BROWSE_Y2 = ZOOM_Y2-25,
//	  };
#define NEWS_BROWSE_X1 (ZOOM_X1+6)
#define NEWS_BROWSE_Y1 (ZOOM_Y1+6)
#define NEWS_BROWSE_X2 (ZOOM_X2-6)
#define NEWS_BROWSE_Y2 (ZOOM_Y2-25)

//----------- Define static variables ----------//

static VBrowseIF browse_news;

//----------- Define static functions ----------//

static void put_news_rec(int recNo, int x, int y, int refreshFlag);

//--------- Begin of function Info::disp_news_log ---------//
//
void Info::disp_news_log(int refreshFlag)
{
	int x=NEWS_BROWSE_X1+9;
	int y=NEWS_BROWSE_Y1+4;

	if( refreshFlag == INFO_REPAINT )
	{
		browse_news.init( NEWS_BROWSE_X1, NEWS_BROWSE_Y1, NEWS_BROWSE_X2, NEWS_BROWSE_Y2,
								0, 32, news_array.size(), put_news_rec, 1 );

		browse_news.open(browse_news_recno);
	}
	else
	{
		browse_news.paint();
		browse_news.open(browse_news_recno, news_array.size());
	}

	//------- display button ---------//

	image_icon.put_back(ZOOM_X2-27, ZOOM_Y2-23, "NEWS_LOG");	// news log report
}
//----------- End of function Info::disp_news_log -----------//


//--------- Begin of function Info::detect_news_log ---------//
//
void Info::detect_news_log()
{
	if( browse_news.detect() )
		browse_news_recno = browse_news.recno();

	//--------- detect button --------//

	if( mouse.single_click( ZOOM_X2-27, ZOOM_Y2-23, ZOOM_X2-27+14, ZOOM_Y2-23+14 ) )
		sys.set_view_mode(MODE_NORMAL);
}
//----------- End of function Info::detect_news_log -----------//


//-------- Begin of static function put_news_rec --------//
//
static void put_news_rec(int recNo, int x, int y, int refreshFlag)
{
#if(defined(CHINESE))
	//SXM
	y+=4;
	//SXM
#endif

	News* newsPtr = news_array[ news_array.size()-recNo+1 ]; 	// display in reversed order

	font_bld.put( x+20, y, date.date_str(newsPtr->news_date, 1) );

   talk_res.msg_add_nation_color = 1; 
	font_bld.put_paragraph( x+110, y, browse_news.ix2-30, y+30, newsPtr->msg() );
	talk_res.msg_add_nation_color = 0;

	//----- display and detect the go icon -----//

	if( newsPtr->is_loc_valid() )
	{
		image_icon.put_back( x+2, y+1, "NEWS_GO" );

		if( mouse.single_click(x+2, y+1, x+17, y+16) )
		{
			vga.use_front();
			world.go_loc( newsPtr->loc_x, newsPtr->loc_y, 1 );		// 1-select object on the location if there is any
			vga.use_back();
		}
	}
}
//----------- End of static function put_news_rec -----------//


