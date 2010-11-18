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

// Filename    : OSEDITOR.H
// Description : Scenario editor


#ifndef __OSEDITOR_H
#define __OSEDITOR_H


#include <OPLASMA.H>
//#include <OEND_CON.H>
#include <GAMEDEF.H>

// enum for edit_mode

enum
{
	SEDIT_MODE_NONE = 0,
//	SEDIT_MODE_TERRAIN,
	SEDIT_MODE_HUMAN,
	SEDIT_MODE_MONSTER,
	SEDIT_MODE_MISC,
	SEDIT_MODE_EDIT,
//	SEDIT_MODE_DEFINE,
	SEDIT_MODE_PLAYER,
	SEDIT_MODE_DIPLOMACY,
	SEDIT_MODE_VICTORY,
	SEDIT_MODE_HERO,

	SEDIT_MODE_COUNT,
};

// ----- define constant -------//

//#define MAX_WIN_CONDITION 8
//#define MAX_LOSE_CONDITION 8

// ----- define class ScenarioEditor ---------//

class ScenarioEditor
{
public:
	char	init_flag;
	char	enable_flag;
	char	open_flag;

	char	edit_mode;
	char	edit_sub_menu_mode[SEDIT_MODE_COUNT];
	int	selected_nation;

	// ----- terrain data --------- //

//	Plasma	height_map, terrain_map, road_map, hill_map, plant_map;
//	int		brush_size;
//	int		brush_pressure;
//	int		brush_type;

	// ----- human unit data --------//

	int		unit_or_struct;
	int		brush_player_recno;

	int		human_unit_id_count;
	short*	human_unit_id_array;
	int		human_unit_id_browse_recno;
	int		human_firm_group_count;
	short*	human_firm_group_array;
	int		human_firm_group_browse_recno;
	int		human_race_filter;

	// ----- monster unit data --------//

	int		monster_unit_id_count;
	short*	monster_unit_id_array;
	int		monster_unit_id_browse_recno;
	int		monster_firm_group_count;
	short*	monster_firm_group_array;
	int		monster_firm_group_browse_recno;
	int		monster_race_filter;

	// ----- hero unit data --------//
	int		inf_or_spu;

	int		hero_id_count;
	short*	hero_id_array;
	short*	hero_unit_id_array;
	int		hero_id_browse_recno;
	int		hero_race_filter;
	
	//------ misc mode data ---------//

	int		rock_or_site;
	int		rock_type;
	int		site_type;
	int		rock_browse_count[4];
	short*	rock_browse_array[4];
	int		rock_browse_recno[4];
	int		raw_browse_recno;
	int		scroll_browse_recno;
	int		gold_browse_recno;
	int		item_browse_recno;
	int		tech_browse_recno;
	int		plant_browse_recno;
	int		plant_size;

	// ------ edit mode data -------//

	int		edit_mode_flag;         // flag indicate edit mode to indicate function

	// ------ player mode data -------//

	// ------- define mode data -------//

//	long		nation_area[MAX_NATION];
//	int		area_brush_size;
//	int		area_brush_nation;

	// ----- diplomacy mode data -----//

	// ----- victory mode data -------//

//	int		win_condition_count;
//	int		lose_condition_count;
//	EndCondition win_condition[MAX_WIN_CONDITION];
//	EndCondition lose_condition[MAX_LOSE_CONDITION];

	int		win_condition_browse_recno;
	int		lose_condition_browse_recno;
	int		win_id_browse_recno;
	int		lose_id_browse_recno;
	int		unit_id_browse_recno;
	int		item_id_browse_recno;
	int		tech_id_browse_recno;
	int		win_or_lose;

public:
	ScenarioEditor();
	~ScenarioEditor();

	void	init();
	void	deinit();

	void	enable();
	void	disable();
	int	is_enable()		{ return init_flag && enable_flag; }
	int	is_edit_mode();

	// ------ init function of each mode --------//

//	void	init_terrain_mode();
//	void	deinit_terrain_mode();

	void	init_human_mode();
	void	deinit_human_mode();

	void	init_monster_mode();
	void	deinit_monster_mode();

	void	init_misc_mode();
	void	deinit_misc_mode();

	void	init_edit_mode();
	void	deinit_edit_mode();

//	void	init_define_mode();
//	void	deinit_define_mode();

	void	init_player_mode();
	void	deinit_player_mode();

	void	init_diplomacy_mode();
	void	deinit_diplomacy_mode();

	void	init_victory_mode();
	void	deinit_victory_mode();

	void	init_hero_mode();
	void	deinit_hero_mode();

	// -------- menu bar interface functions -------- //

	void	disp_menu_bar(int refreshFlag);
	int	detect_menu_bar();

	// -------- interface function ------------//

	void	disp(int refreshFlag);
	void	detect();

//	void	disp_terrain_main(int refreshFlag);
//	void	detect_terrain_main();

	void	disp_human_main(int refreshFlag);
	void	detect_human_main();

	void	disp_monster_main(int refreshFlag);
	void	detect_monster_main();

	void	disp_misc_main(int refreshFlag);
	void	detect_misc_main();

	void	disp_edit_main(int refreshFlag);
	void	detect_edit_main();

//	void	disp_define_main(int refreshFlag);
//	void	detect_define_main();

	void	disp_players_main(int refreshFlag);
	void	detect_players_main();

	void	disp_diplomacy_main(int refreshFlag);
	void	detect_diplomacy_main();

	void	disp_victory_main(int refreshFlag);
	void	detect_victory_main();

	void	disp_hero_main(int refreshFlag);
	void	detect_hero_main();

	// ------ disp/detect view function --------//

	void	disp_view();
	int	detect_view();

//	void	disp_terrain_view();
//	int	detect_terrain_view();

	void	disp_human_view();
	int	detect_human_view();

	void	disp_monster_view();
	int	detect_monster_view();

	void	disp_misc_view();
	int	detect_misc_view();

	void	disp_edit_view();
	int	detect_edit_view();

//	void	disp_define_view();
//	int	detect_define_view();

	void	disp_players_view();
	int	detect_players_view();

	void	disp_diplomacy_view();
	int	detect_diplomacy_view();

	void	disp_victory_view();
	int	detect_victory_view();

	void	disp_hero_view();
	int	detect_hero_view();

private:

	void	draw_marker( int validAction, int x1, int y1, int x2, int y2 );

	// used by human and fryhtan mode
	void	collect_human_unit( int raceId );
	void	collect_human_firm( int raceId );
	void	collect_monster_unit( int raceId );
	void	collect_monster_firm( int raceId );
	void	collect_hero_unit( int raceId );
};

extern ScenarioEditor scenario_editor;

#endif