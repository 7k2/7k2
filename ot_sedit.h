// Filename    : OT_SEDIT.H
// Description : Text resource for scenario editor


#ifndef __OT_SEDIT_H
#define __OT_SEDIT_H

#include <OTXTRES.H>

class TextResEditor : public TextResource
{
public:
	void	init();

	// main
	char*	str_mode(int editorModeId );
	char* str_editor_menu();
	char* str_blacken_map();

	// human mode
	char* str_unit();
	char* str_structure();
	char* str_human();
	char* str_race_town( char *raceAdj );
	char* str_all();
	char* str_no_independent();
	char* str_double_left_add();
	char* str_double_right_del();

	// fryhtan mode
	char* str_no_human();

	// misc mode
	char* str_misc_mode( int, int =-1 );

	// player mode
	char* str_player_treasure();
	char* str_player_food();
	char* str_player_reputation();
	char* str_player_live_points();
	char* str_king_name();
	char* str_fine_coarse(int);
	char* str_player_disable();
	char* str_adjustment();

	// relation status
	char* str_abbrev_relation_str(int);
	char* str_abbrev_trade_treaty();
	char* str_relation_inst();

	// victory mode
	char* str_victory();
	char* str_loss();
	char* str_add();
	char* str_delete();
	char* str_require_all();
	char* str_require_any();
	char* str_next();
	char* str_cancel();
	char* str_time_limit( int years);
	char* str_no_time_limit();
	char* str_back();
	char* str_select_nation();
	char* str_select_unit();
	char* str_select_friendly_unit();
	char* str_select_town();
	char* str_select_friendly_town();
	char* str_select_firm();
	char* str_select_friendly_firm();
	char* str_select_item();
	char* str_deadline();

	// hero mode
	char* str_hero_infantry();
	char* str_hero_spu();
};

extern TextResEditor text_editor;

#endif