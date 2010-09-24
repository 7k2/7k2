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