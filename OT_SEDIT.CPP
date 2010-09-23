// Filename    : OT_SEDIT.H
// Description : Text resource for scenario editor

#include <OT_SEDIT.H>
#include <ALL.H>
#include <OSEDITOR.H>
#include <ONATIONB.H>

 
void TextResEditor::init()
{
	TextResource::init( DIR_RES"T_SEDIT.RES" );
}


// text id 1 - 9 (10-14 reserved)
// [SEM0]Off
// [SEM1]Human Objects
// [SEM2]Fryhtan Objects
// [SEM3]Misc. Objects
// [SEM4]Attributes Editor
// [SEM5]Players Information
// [SEM6]Diplomacy Relations
// [SEM7]Victory Conditions
// [SEM8]Hero Editor
char*	TextResEditor::str_mode(int editorModeId )
{
	err_when( editorModeId < 0 || editorModeId > 8 );

	if( editorModeId == SEDIT_MODE_HUMAN )
	{
		return get_format_str( 1+editorModeId, "SEM1" );
	}
	return get_format_str( 1+editorModeId, NULL );
}

// text id 15
// [BLKM]Blacken Map
char* TextResEditor::str_blacken_map()
{
	return get_format_str( 15, "BLKM" );
}

// text id 16
// [SCEM]Scenario Editor Menu
char* TextResEditor::str_editor_menu()
{
	return get_format_str( 16, "SCEM" );
}

// text id 17
// [UNIT]Unit
char* TextResEditor::str_unit()
{
	return get_format_str( 17, "UNIT" );
}

// text id 18
// [STRU]Structure
char* TextResEditor::str_structure()
{
	return get_format_str( 18, "STRU" );
}

// text id 19
// [HUMA]Human
char* TextResEditor::str_human()
{
	return get_format_str( 19, "HUMA" );
}

// text id 20
// [RTWN]%1s town
char* TextResEditor::str_race_town( char *raceAdj )
{
	return process( get_format_str( 20, "RTWN" ),
		raceAdj );
}

// text id 21
// [ALLR]All
char* TextResEditor::str_all()
{
	return get_format_str( 21, "ALLR" );
}

// text id 22
// [NOIN]Cannot add any Independent structures
char* TextResEditor::str_no_independent()
{
	return get_format_str( 22, "NOIN" );
}

// text id 23
// [NOHU]Human Kingdom cannot possess Fryhtan structure
char* TextResEditor::str_no_human()
{
	return get_format_str( 23, "NOHU" );
}

// text id 24 - 26 (27 - 31 reserved)
// [MIM0]Terrain Objects
// [MIM1]Items
// [MIM2]Plant
//
// sub modes of terrain object mode
// text id 32-35 (36-39 reserved)
// [MI00]Rock
// [MI01]Dirt
// [MI02]Flat Rock
// [MI03]Lake
// 
char* TextResEditor::str_misc_mode( int miscMode, int miscSubMode )
{
	switch( miscMode )
	{
	case 0:		// misc/terrain mode
		if( miscSubMode < 0 )
			return get_format_str( 24, "MIM0" );
		return get_format_str( 32+miscSubMode, NULL );
	case 1:		// misc/item mode
		if( miscSubMode == -1 )
			return get_format_str( 25, "MIM1" );
		return get_format_str( 40+miscSubMode, NULL );
	case 2:		// misc/plant mode
		if( miscSubMode == -1 )
			return get_format_str( 26, "MIM2" );
		return get_format_str( 48+miscSubMode, NULL );
	}
	
	err_here();
	return "";
}

// text id 56 - 71 reserved

// text id 72
// [PTRE]Treasure
char* TextResEditor::str_player_treasure()
{
	return get_format_str( 72, "PTRE" );
}

// text id 73
// [PFOO]Food
char* TextResEditor::str_player_food()
{
	return get_format_str( 73, "PFOO" );
}

// text id 74
// [PREP]Reputation
char* TextResEditor::str_player_reputation()
{
	return get_format_str( 74, "PREP" );
}

// text id 75
// [PLPS]Life Points
char* TextResEditor::str_player_live_points()
{
	return get_format_str( 75, "PLPS" );
}

// text id 76
// [KNAM]King Name
char* TextResEditor::str_king_name()
{
	return get_format_str( 76, "KNAM" );
}

// text id 77 - 79
// [FINE]Fine
// [MEDF]Medium
// [COAR]Coarse
char* TextResEditor::str_fine_coarse(int i)
{
	switch(i)
	{
	case 0:
		return get_format_str( 77, "FINE" );
	case 1:
		return get_format_str( 78, "MEDF" );
	case 2:
		return get_format_str( 79, "COAR" );
	default:
		err_here();
		return "";
	}
}

// text id 80
// [DISB](Disabled)
char* TextResEditor::str_player_disable()
{
	return get_format_str( 80, "DISB" );
}

// text id 81
// [ADD_]Add
char* TextResEditor::str_add()
{
	return get_format_str( 81, "ADD_" );
}

// text id 82
// [DELE]Delete
char* TextResEditor::str_delete()
{
	return get_format_str( 82, "DELE" );
}

// text id 83
// [REQA]All conditions are true
char* TextResEditor::str_require_all()
{
	return get_format_str( 83, "REQA" );
}

// text id 84
// [NEXT]Next
char* TextResEditor::str_next()
{
	return get_format_str( 84, "NEXT" );
}

// text id 85
// [CNCL]Cancel
char* TextResEditor::str_cancel()
{
	return get_format_str( 85, "CNCL" );
}

// text id 86
// [DUYR]%1d %1p{year|years}
char* TextResEditor::str_time_limit( int years)
{
	return process( get_format_str( 86, "DUYR" ),
		years );
}

// text id 87
// [NOLM]No time limit
char* TextResEditor::str_no_time_limit()
{
	return get_format_str( 87, "NOLM" );
}

// text id 88
// [BACK]Back
char* TextResEditor::str_back()
{
	return get_format_str( 88, "BACK" );
}

// text id 89
// [SELN]Select target nation by selecting any object of that nation
char* TextResEditor::str_select_nation()
{
	return get_format_str( 89, "SELN" );
}

// text id 90
// [SELU]Select target unit
char* TextResEditor::str_select_unit()
{
	return get_format_str( 90, "SELU" );
}

// text id 91
// [SELF]Select a friendly unit
char* TextResEditor::str_select_friendly_unit()
{
	return get_format_str( 91, "SELF" );
}

// text id 92
// [SELT]Select target town
char* TextResEditor::str_select_town()
{
	return get_format_str( 92, "SELT" );
}

// text id 93
// [SEFT]Select a friendly town
char* TextResEditor::str_select_friendly_town()
{
	return get_format_str( 93, "SEFT" );
}

// text id 94
// [SELB]Select target building
char* TextResEditor::str_select_firm()
{
	return get_format_str( 94, "SELB" );
}

// text id 95
// [SEFB]Select a friendly building
char* TextResEditor::str_select_friendly_firm()
{
	return get_format_str( 95, "SEFB" );
}

// text id 96
// [SELI]Select target item
char* TextResEditor::str_select_item()
{
	return get_format_str( 96, "SELI" );
}

// text id 97 - 104 reserved


// text id 105
// [HINF]Infantry
char* TextResEditor::str_hero_infantry()
{
	return get_format_str( 105, "HINF" );
}

// text id 106
// [HSPU]Special
char* TextResEditor::str_hero_spu()
{
	return get_format_str( 106, "HSPU" );
}

// text id 107
// [PADJ]Adjustment:
char* TextResEditor::str_adjustment()
{
	return get_format_str( 107, "PADJ" );
}

// text id 108-112
// [REL0]W
// [REL1]U
// [REL2]N
// [REL3]F
// [REL4]A
char* TextResEditor::str_abbrev_relation_str(int r)
{
	switch(r)
	{
	case RELATION_HOSTILE:
		return get_format_str( 108, "REL0" );
	case RELATION_TENSE:
		return get_format_str( 109, "REL1" );
	case RELATION_NEUTRAL:
		return get_format_str( 110, "REL2" );
	case RELATION_FRIENDLY:
		return get_format_str( 111, "REL3" );
	case RELATION_ALLIANCE:
		return get_format_str( 112, "REL4" );
	default:
		return "";
	}
}

// text id 113
// [RELT]t
char* TextResEditor::str_abbrev_trade_treaty()
{
	return get_format_str( 113, "RELT" );
}

// text id 114
// [RELI]Left click or right click to change diplomatic status
char* TextResEditor::str_relation_inst()
{
	return get_format_str( 114, "RELI" );
}

// text id 115
// [BVIC]Victory
char* TextResEditor::str_victory()
{
	return get_format_str( 115, "BVIC" );
}

// text id 116
// [BLOS]Loss
char* TextResEditor::str_loss()
{
	return get_format_str( 116, "BLOS" );
}

// text id 117
// [REQO]Any Condition is true
char* TextResEditor::str_require_any()
{
	return get_format_str( 117, "REQO" );
}

// text id 118
// [DDLN]Time Limit:
char* TextResEditor::str_deadline()
{
	return get_format_str( 118, "DDLN" );
}

// text id 119
// [DLCR]Double left click to create
char* TextResEditor::str_double_left_add()
{
	return get_format_str( 119, "DLCR" );
}

// text id 120
// [DRRM]Double right click to remove
char* TextResEditor::str_double_right_del()
{
	return get_format_str( 120, "DRRM" );
}

