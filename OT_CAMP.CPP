// Filename    : OT_CAMP.CPP
// Description : tutorialgame menu text resource

#include <OT_CAMP.H>
#include <ALL.H>
#include <OCAMPGN.H>


void TextResCampaign::init()
{
	TextResource::init( DIR_RES"T_CAMP.RES" );
}

// text id 1 - 9
// [CAM1]Occidental Herores
// [CAM2]reserved campaign
// ...
// [CAM9]reserved campaign
char* TextResCampaign::str_campaign_name(int campaignId)
{
	switch( campaignId )
	{
	case CAMPAIGN_EAST_WEST:
		return get_format_str( 1, "CAM1" );

	default:
		err_here();
		return "?";
	}
}

// text id 10
// [ITR1]Where once you strode the World assured, you are the Huniers prey
char* TextResCampaign::str_intro_1()
{
	return get_format_str( 10, "ITR1" );
}

// text id 11
// [ITR2]An Empire lost and Pain and Scorn and Death endured
char* TextResCampaign::str_intro_2()
{
	return get_format_str( 11, "ITR2" );
}

// text id 12
// [ITR3]Naught but Years of Torment are assured
char* TextResCampaign::str_intro_3()
{
	return get_format_str( 12, "ITR3" );
}

// text id 13
// [ITR4]Before your Foes can you repay
char* TextResCampaign::str_intro_4()
{
	return get_format_str( 13, "ITR4" );
}

// text id 14
// [ITR5]Now rouse a host and take the field
char* TextResCampaign::str_intro_5()
{
	return get_format_str( 14, "ITR5" );
}

// text id 15
// [ITR6]Choose men of whom canst boast
char* TextResCampaign::str_intro_6()
{
	return get_format_str( 15, "ITR6" );
}

// text id 16
// [ITR7]Did pledge their Liege, their Lives their Ghost
char* TextResCampaign::str_intro_7()
{
	return get_format_str( 16, "ITR7" );
}

// text id 17
// [ITR8]And forced all else to yield
char* TextResCampaign::str_intro_8()
{
	return get_format_str( 17, "ITR8" );
}

// text id 18
// [FRF1]Against you stood the Frythan Lords and their Evil Hoedes arrayed
char* TextResCampaign::str_monster_fall_1()
{
	return get_format_str( 18, "FRF1" );
}

// text id 19
// [FRF2]Their Blood it poured and as one they roared
char* TextResCampaign::str_monster_fall_2()
{
	return get_format_str( 19, "FRF2" );
}

// text id 20
// [FRF3]And screamed in agony dismayed
char* TextResCampaign::str_monster_fall_3()
{
	return get_format_str( 20, "FRF3" );
}

// text id 21
// [FRF4]It is the End. We are by the very Gods betrayed!
char* TextResCampaign::str_monster_fall_4()
{
	return get_format_str( 21, "FRF4" );
}

// text id 22
// [VIC1]Regain lands lost is your forefather's day, all this you did avow
char* TextResCampaign::str_victory_1()
{
	return get_format_str( 22, "VIC1" );
}

// text id 23
// [VIC2]Though men of steel and steeds of gray
char* TextResCampaign::str_victory_2()
{
	return get_format_str( 23, "VIC2" );
}

// text id 24
// [VIC3]Stood hard and fast and barred your way
char* TextResCampaign::str_victory_3()
{
	return get_format_str( 24, "VIC3" );
}

// text id 25
// [VIC4]Yet tis you to who all heads now bow
char* TextResCampaign::str_victory_4()
{
	return get_format_str( 25, "VIC4" );
}

// text id 26
// [VIC5]Against foes both Beast and Man
char* TextResCampaign::str_victory_5()
{
	return get_format_str( 26, "VIC5" );
}

// text id 27
// [VIC6]you stood beneath your Flag Unfurled
char* TextResCampaign::str_victory_6()
{
	return get_format_str( 27, "VIC6" );
}

// text id 28
// [VIC7]They numbered as the Grains of Stand
char* TextResCampaign::str_victory_7()
{
	return get_format_str( 28, "VIC7" );
}

// text id 29
// [VIC8]In the Dead Seas and the Desert Land
char* TextResCampaign::str_victory_8()
{
	return get_format_str( 29, "VIC8" );
}

// text id 30
// [CAMS]Campaign Score To Date : %1d
char* TextResCampaign::str_campaign_score(int campaignScore)
{
	return process( get_format_str( 30, "CAMS"), campaignScore );
}

// text id 31
// [EVEN]Event : %1d (Run count: %2d)
char* TextResCampaign::str_event_count(int eventId, int totalEventRunCount)
{
	return process( get_format_str( 31, "EVEN"), eventId, totalEventRunCount );
}

// text id 32
// [STAG]Stage : %1d (Run count: %2d)
char* TextResCampaign::str_stage_count(int stageId, int totalStageRunCount)
{
	return process( get_format_str( 32, "STAG"), stageId, totalStageRunCount );
}

// text id 33
// [SCOR]Score
char* TextResCampaign::str_score()
{
	return get_format_str( 33, "SCOR" );
}

// text id 34
// [DIFR]Difficulty Rating
char* TextResCampaign::str_difficulty_rating()
{
	return get_format_str( 34, "DIFR" );
}

// text id 35
// [CLCK]Click to continue.
char* TextResCampaign::str_click_to_continue()
{
	return get_format_str( 35, "CLCK" );
}

// text id 36
// [BONU]Your bonus, to be added to your campaign score, is %1d
char* TextResCampaign::str_bonus(int bonus)
{
	return process( get_format_str( 36, "BONU"), bonus );
}

// text id 37
// [BONF]That is %1d, minus the number of years that it took to achieve the goal.
char* TextResCampaign::str_bonus_formula(int maxYear)
{
	return process( get_format_str( 37, "BONF"), maxYear );
}

// text id 38
// [CLKM]Click to read message...
char* TextResCampaign::str_click_to_read_msg()
{
	return get_format_str( 38, "CLKM" );
}

// text id 39
// [ROYU]Royal Units
char* TextResCampaign::str_royal_units()
{
	return get_format_str( 39, "ROYU" );
}

// text id 40
// [RHLP]Check the units that you wish to use in this scenario. You may Click on an Artifact and move it to another unit.
char* TextResCampaign::str_select_royal_help_1()
{
	return get_format_str( 40, "RHLP" );
}

// text id 41
// [RHL2]Reserved
char* TextResCampaign::str_select_royal_help_2()
{
	return get_format_str( 41, "RHL2" );
}

// text id 42
// [DONE]Done
char* TextResCampaign::str_done()
{
	return get_format_str( 42, "DONE" );
}

// text id 43
// [REST]Restore
char* TextResCampaign::str_restore()
{
	return get_format_str( 43, "REST" );
}

// text id 44
// [RNDS]Random Select
char* TextResCampaign::str_random_select()
{
	return get_format_str( 44, "RNDS" );
}

// text id 45
// [DALL]Deselect All
char* TextResCampaign::str_deselect_all()
{
	return get_format_str( 45, "DALL" );
}

// text id 46
// [DELR]Delete
char* TextResCampaign::str_delete_royal()
{
	return get_format_str( 46, "DELR" );
}

// text id 47
// [HU_F]Human Only
char* TextResCampaign::str_human_force()
{
	return get_format_str( 47, "HU_F" );
}

// text id 48
// [FR_F]Human & Fryhtan
char* TextResCampaign::str_fryhtan_force()
{
	return get_format_str( 48, "FR_F" );
}

// text id 49
// [ASKF]Choose the type of force that you wish to field in the next mission:
char* TextResCampaign::str_ask_fryhtan()
{
	return get_format_str( 49, "ASKF" );
}

// text id 50
// [MXRF]You cannot bring more than %1d royal units to scenario
char* TextResCampaign::str_max_royal_on_field(int maxRoyal)
{
	return process( get_format_str( 50, "MXRF" ), maxRoyal );
}

// text id 51
// [MXRS]You cannot keep more than %1d royal units, delete some of them
char* TextResCampaign::str_max_royal_in_store(int maxRoyal)
{
	return process( get_format_str( 51, "MXRS" ), maxRoyal );
}

// text id 52
// [D.I?]Drop this item?
char* TextResCampaign::str_confirm_drop_item()
{
	return get_format_str( 52, "D.I?" );
}

// text id 53
// [E.R?]Are you sure?
char* TextResCampaign::str_confirm_end_royal()
{
	return get_format_str( 53, "E.R?" );
}

// text id 54
// [EASE]Eastern Empire
char* TextResCampaign::str_eastern_empire()
{
	return get_format_str( 54, "EASE" );
}

// text id 55
// [WESE]Western Empire
char* TextResCampaign::str_western_empire()
{
	return get_format_str( 55, "WESE" );
}


// text id 56 - 70 reserved

// text id 71
// [ATKS]Please select a state to attack
char* TextResCampaign::str_select_attack_state()
{
	return get_format_str( 71, "ATKS" );
}

// text id 72
// [<kingdom>STON]State of %1s
char* TextResCampaign::str_state_of_cnation( char* cNationName )
{
	return process( get_format_str( 72, "STON" ),
		cNationName );
}

// text id 73
// [STIN]Independent state
char* TextResCampaign::str_independent_state()
{
	return get_format_str( 73, "STIN" );
}

// text id 74
// [CATK]Click to Attack
char* TextResCampaign::str_click_to_attack()
{
	return get_format_str( 74, "CATK" );
}

// text id 75
// [NATK]Cannot select this state to attack
char* TextResCampaign::str_cannot_attack()
{
	return get_format_str( 75, "NATK" );
}

// text id 76
// [VIC9]Yet tis you who rules this grateful World
char* TextResCampaign::str_victory_9()
{
	return get_format_str( 76, "VIC9" );
}

