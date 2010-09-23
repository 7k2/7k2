//Filename    : ONATION.CPP
//Description : Object Nation - part 1

#include <OSTR.H>
#include <OSYS.H>
#include <OSERES.H>
#include <ODATE.H>
#include <OBOX.H>
#include <OVGA.H>
#include <OFONT.H>
#include <ONEWS.H>
#include <OWORLD.H>
#include <OFIRM.H>
#include <OF_CAMP.H>
#include <OF_TRAIN.H>
#include <OU_WAGON.H>
#include <OGAME.H>
#include <OPOWER.H>
#include <OREGIONS.H>
#include <ORACERES.H> 
#include <OTALKRES.H>
#include <OMONSRES.H>
#include <OGODRES.H>
#include <OTOWN.H>
#include <OSPY.H>
#include <OF_MARK.H>
#include <OF_BASE.H>
#include <OTECH.H>
#include <OTECHRES.H>
#include <ONATION.H>
#include <OREBEL.H>
#include <OREMOTE.H>
#include <OCONFIG.H>
#include <OTUTOR.H>
#include <OBULLET.H>
#include <math.h>
#include <OSE.H>
#include <OT_UNIT.H>
#include <OT_REPS.H>
#include <OT_TALK.H>

//-------- Define static variables --------//

//char* NationRelation::relation_status_str_array[5] =
//{
//	"War", "Unfriendly", "Neutral", "Friendly", "Alliance"
//};

//--------- Define static functions -------//

static int succeed_king_loyalty_change(int thisRaceId, int newKingRaceId, int oldKingRaceId);

//--------- Begin of function NationBase::NationBase --------//

NationBase::NationBase()
{
	memset( this, 0, sizeof(NationBase) );
}
//---------- End of function NationBase::NationBase --------//


//--------- Begin of function NationBase::~NationBase --------//

NationBase::~NationBase()
{
	if( sys.quick_exit_flag() )
		return;

#ifdef DEBUG

	err_when( nation_recno );		// deinit() must be called first before this destructor is called

	for( int i=0 ; i<MAX_RACE ; i++ )
		err_when( base_count_array[i] > 0 );		// it should be all zeros

#endif
}
//---------- End of function NationBase::~NationBase --------//


//--------- Begin of function NationBase::init --------//
//
// This function will be called directly by
//
// <int>   nationType    = the nation type (NATION_???)
// <int>   raceId      	 = id. of the race
// <int>	  colorSchemeId = color scheme id. of the nation
// [DWORD] playerId      = an unique player id. (for multiplayer game)
//
void NationBase::init(int nationType, int raceId, int colorSchemeId, DWORD playerId)
{
	//------------- set vars ---------------//

	nation_type  	 = nationType;
	race_id 	    	 = raceId;
	color_scheme_id = colorSchemeId;
	player_id    	 = playerId;

	colorSchemeId	 = min( colorSchemeId, MAX_COLOR_SCHEME );
	nation_color	 = game.color_remap_array[colorSchemeId].main_color;

	last_war_date   = info.game_date;

	//------- if this is the local player's nation -------//

	if( nation_type==NATION_OWN )
		nation_array.player_recno = nation_recno;


	//------- reset custom names --------//

	nation_array.set_custom_king_name(nation_recno, "");
	nation_array.set_custom_nation_name(nation_recno, "");


	//---------- init game vars ------------//

	init_cash(0);		// no gem stones passed

	//---- initialize this nation's relation on other nations ----//

	for( int i=nation_array.size() ; i>0 ; i-- )
	{
		if( nation_array.is_deleted(i) )
			continue;

		init_relation(i);
		nation_array[i]->init_relation(nation_recno);
	}

	//--------- init technology ----------//

	tech_res.init_nation_tech(nation_recno);

	// ###### begin Gilbert 10/12 ########//

	if( config.spy_methodology )
	{
		tech_res.inc_tech_class_level(nation_recno, TECH_CLASS_SPYING);
	}
	// ###### end Gilbert 10/12 ########//

	//------- reset all god knowledge --------//

	god_res.init_nation_know(nation_recno);

	//### begin alex 23/9 ###//
	if(remote.is_enable() && nation_recno && !is_ai() && m.is_file_exist("TECHGOD.SYS"))
	{
		tech_res.inc_all_tech_level(nation_recno);
		tech_res.inc_all_tech_level(nation_recno);
		tech_res.inc_all_tech_level(nation_recno);
		god_res.enable_know_all(nation_recno);
	}
	//#### end alex 23/9 ####//
}
//----------- End of function NationBase::init ---------//


//--------- Begin of function NationBase::deinit --------//
//
// When a nation is to be deleted, NationBase::deinit() must
// be called first from nation_array before calling its
// destructor as when it deinitalizes units, some functions
// will need to access the nation_array[].
//
void NationBase::deinit()
{
	if( sys.quick_exit_flag() )
	{
		nation_recno = 0;
		return;
	}

	//-------------------------------------//

	if( nation_recno==0 )		// has been deinitialized
		return;

	//---- delete all talk messages to/from this nation ----//

	talk_res.del_all_nation_msg(nation_recno);

	//------- close down all firms --------//

	close_all_firm();

	//---- neutralize all towns belong to this nation ----//

	Town* townPtr;

	for( int i=town_array.size() ; i>0 ; i-- )
	{
		if( town_array.is_deleted(i) )
			continue;

		townPtr = town_array[i];

		if( townPtr->nation_recno == nation_recno )
			townPtr->set_nation(0);
	}

	//------------- deinit our spies -------------//

	for( i=spy_array.size() ; i>0 ; i-- )
	{
		if( spy_array.is_deleted(i) )
			continue;

		Spy* spyPtr = spy_array[i];

		//-----------------------------------------------------//
		// Convert all of spies of this nation to normal units, 
		// so there  will be no more spies of this nation.
		//-----------------------------------------------------//

		if( spyPtr->true_nation_recno == nation_recno )		// drop spy identities of spies in towns, firms and mobile ones
			spyPtr->drop_spy_identity();	

		//-----------------------------------------------------//
		// For spies of other nation cloaked as this nation,
		// their will uncover their cloak and change back
		// to their original nation. 
		//-----------------------------------------------------//

		// ###### begin Gilbert 4/7 ########//
		else if( spyPtr->cloaked_nation_recno == nation_recno )
		{
			int oldSpyPlace = spyPtr->spy_place;

			spyPtr->change_cloaked_nation(spyPtr->true_nation_recno);

			// if a spy in a wagon, he will be mobilized to an dividual unit
			// now go back to town
			if( oldSpyPlace == UNIT_WAGON && spyPtr->spy_place == SPY_MOBILE )
			{
				unit_array[spyPtr->spy_place_para]->ai_move_to_nearby_town();
			}
		}
		// ###### end Gilbert 4/7 ########//

		err_when( spyPtr->true_nation_recno == nation_recno ||		// there should be no more spies associated with this nation 
				    spyPtr->cloaked_nation_recno == nation_recno );
	}

	//----- deinit all units belonging to this nation -----//

	deinit_all_unit();

	//-------- if the viewing nation is this nation -------//

	if( !sys.signal_exit_flag )
	{
		if( info.default_viewing_nation_recno == nation_recno )
		{
			info.default_viewing_nation_recno = nation_array.player_recno;
			sys.set_view_mode(MODE_NORMAL);
		}

		else if( info.viewing_nation_recno == nation_recno )
			sys.set_view_mode(MODE_NORMAL); 		// it will set viewing_nation_recno to default_viewing_nation_recno

		// ##### begin Gilbert 22/10 #######//
		// if deleting own nation, darken view mode buttons
		if( nation_recno == nation_array.player_recno )
		{
			sys.disp_view_mode(1);
		}
		// ##### end Gilbert 22/10 #######//
	}

	nation_recno = 0;
}
//----------- End of function NationBase::deinit ---------//


//--------- Begin of function NationBase::init_cash --------//
//
void NationBase::init_cash(int gemStones)
{
	//---------- init game vars ------------//

	static int start_up_cash_array[] = { 4000, 7000, 12000, 20000 };
	static int start_up_live_array[] = {  800, 1500,  2500,  4000 };


	if( is_ai() )
		cash = (float) start_up_cash_array[config.ai_start_up_cash-1];
	else
		cash = (float) start_up_cash_array[config.start_up_cash-1];

	food = (float) 5000;			// startup food is 5000 for all nations in all settings

	if( is_monster() )
	{
		food /= 2;
		cash /= 2;
		live_points = (float) start_up_live_array[config.start_up_cash-1];
	}

	if( gemStones )
	{
		err_when( gemStones <= -10 );
		cash += cash * 0.15f * gemStones;
		live_points += live_points * 0.15f * gemStones;
	}
}
//----------- End of function NationBase::init_cash ---------//


//--------- Begin of function NationBase::init_relation --------//
//
// Initialize the relation vars with the given nation.
//
// <int> relationNationRecno - recno of the NationRelation to set.
//
void NationBase::init_relation(int relationNationRecno)
{
	NationRelation* nationRelation = relation_array+relationNationRecno-1;

	memset( nationRelation, 0, sizeof(NationRelation) );

	set_relation_should_attack(relationNationRecno, relationNationRecno!=nation_recno, COMMAND_AUTO);

	if( is_ai() && nation_array[relationNationRecno]->is_ai() )		// AI has contact with each other in the beginning of the game.
		nationRelation->has_contact = 1;
	else
		nationRelation->has_contact = relationNationRecno==nation_recno || config.explore_whole_map;              // if the map is blackened out, no contact in the beginning

	nationRelation->trade_treaty = relationNationRecno==nation_recno;

	nationRelation->status       		 = RELATION_NEUTRAL;
	nationRelation->ai_relation_level = RELATION_NEUTRAL * RELATION_LEVEL_PER_STATUS + m.random(40) - 20;		// -20 to +20 randomness
	nationRelation->last_change_status_date = info.game_date;

	if( relationNationRecno == nation_recno )		// own nation
		relation_status_array[relationNationRecno-1] = RELATION_ALLIANCE;		// for facilitating searching
	else
		relation_status_array[relationNationRecno-1] = RELATION_NEUTRAL;

	set_relation_passable(relationNationRecno, RELATION_FRIENDLY);

	is_allied_with_player = 0;
}
//----------- End of function NationBase::init_relation ---------//


//--------- Begin of function NationBase::close_all_firm --------//
//
// Close down all firms under this nation.
//
void NationBase::close_all_firm()
{
	int i;

	for( i=firm_array.size() ; i>0 ; i-- )
	{
		if( !firm_array.is_deleted(i) &&
			 firm_array[i]->nation_recno == nation_recno )
		{
			firm_array.del_firm(i);
		}
	}
}
//----------- End of function NationBase::close_all_firm ---------//


//--------- Begin of function NationBase::deinit_all_unit --------//
//
// Deinit all units belonging to this nation when this nation is deinitialized.
//
void NationBase::deinit_all_unit()
{
	//--- update total_human_unit so the numbers will be correct ---//

	#ifndef DEBUG  		// only do this in release version to on-fly fix bug
	nation_array.update_statistic();
	#endif

	//--------------------------------------//

	int   i;
	Unit* unitPtr;

	for( i=unit_array.size() ; i>0 ; i-- )
	{
		if( unit_array.is_deleted(i) )
		{
			unitPtr = (Unit*) unit_array.get_ptr(i);		// this unit is dying, so is_deleted() return 1;
			
			if( !unitPtr )
				continue;

			if( unitPtr->nation_recno == nation_recno )
				unit_array.del(i);

			continue;
		}

		//--------------------------------------------//

		unitPtr = unit_array[i];

		if( unitPtr->nation_recno != nation_recno )
			 continue;

		//----- only human units will betray -----//

		if( unitPtr->race_id )
		{
			unitPtr->loyalty = 0;		// force it to detray

			if( unitPtr->think_betray() )
				continue;
		}

		//--- if the unit has not changed nation, the unit will disappear ---//

		if( unit_array.get_ptr(i) )
			unit_array.del(i);
	}

	//------------ debug code -------------//

	for( i=unit_array.size() ; i>0 ; i-- )
	{
		if( unit_array.is_deleted(i) )
			continue;

		unitPtr = unit_array[i];
		
		err_when( unitPtr->nation_recno == nation_recno );		// they should have been all deleted. 
	}
}
//----------- End of function NationBase::deinit_all_unit ---------//


//--------- Begin of function NationBase::hand_over_to --------//
//
// <int> handoverNationRecno - hand over the entire nation to this nation.
//
void NationBase::hand_over_to(int handoverNationRecno)
{
	rebel_array.stop_attack_nation(nation_recno);
	town_array.stop_attack_nation(nation_recno);
	unit_array.stop_all_war(nation_recno);

	nation_hand_over_flag = nation_recno;

	//--- hand over units (should hand over units first as you cannot change a firm's nation without changing the nation of the overseer there ---//

	Unit* unitPtr;

	for( int i=unit_array.size() ; i>0 ; i-- )
	{
		if( unit_array.is_deleted(i) )
		{
			//-- If the unit is dying and isn't truly deleted yet, delete it now --//

			if( !unit_array.is_truly_deleted(i) &&
				 unit_array[i]->nation_recno == nation_recno )
			{
				unit_array.die(i);
			}

			continue;
		}

		//---------------------------------------//

		unitPtr = unit_array[i];

		if( unitPtr->nation_recno != nation_recno )
			continue;

		//----- if it is a god, resign it -------//

		if( god_res.is_god_unit(unitPtr->unit_id) )
		{
			unitPtr->resign(COMMAND_AUTO);
			continue;
		}

		//----- if it is a spy cloaked as this nation -------//
		//
		// If the unit is a overseer of a Camp or Base, 
		// the Camp or Base will change nation as a result. 
		//
		//---------------------------------------------------//

		if( unitPtr->spy_recno )					
			unitPtr->spy_change_nation(handoverNationRecno, COMMAND_AUTO);
		else
			unitPtr->change_nation(handoverNationRecno);
	}

	//------- hand over firms ---------//

	for( i=firm_array.size() ; i>0 ; i-- )
	{
		if( !firm_array.is_deleted(i) &&
			 firm_array[i]->nation_recno == nation_recno )
		{
			firm_array[i]->change_nation(handoverNationRecno);
		}
	}

	//------- hand over towns ---------//

	for( i=town_array.size() ; i>0 ; i-- )
	{
		if( !town_array.is_deleted(i) &&
			 town_array[i]->nation_recno == nation_recno )
		{
			town_array[i]->set_nation(handoverNationRecno);
		}
	}

	// ######## begin Gilbert 9/5 #########//
	// -------- bullet -------------//

	for( i=bullet_array.size(); i > 0; i-- )
	{
		if( !bullet_array.is_deleted(i) &&
			bullet_array[i]->nation_recno == nation_recno )
		{
			bullet_array[i]->change_nation(handoverNationRecno);
		}
	}
	// ######## end Gilbert 9/5 #########//

	//-------------------------------------------------//
	//
	// For the spies of this nation cloaked into other nations,
	// we need to update their true_nation_recno. 
	//
	//-------------------------------------------------//

	Spy* spyPtr;

	for( i=spy_array.size() ; i>0 ; i-- )
	{
		if( spy_array.is_deleted(i) )
			continue;

		spyPtr = spy_array[i];

		err_when( spyPtr->cloaked_nation_recno == nation_recno );		// there should be no units in this nation after the above deinitialization

		if( spyPtr->true_nation_recno == nation_recno )
			spyPtr->change_true_nation(handoverNationRecno);
	}

	//------- delete this nation from nation_array -------//

	nation_array.del_nation(nation_recno);
	nation_hand_over_flag = 0;
}
//----------- End of function NationBase::hand_over_to ---------//


//--------- Begin of function NationBase::set_king --------//
//
// Set the king unit recno.
//
// <int> kingUnitRecno - unit recno of the king
// <int> firstKing - whether this is the first king of the nation.
//
void NationBase::set_king(int kingUnitRecno, int firstKing)
{
	king_unit_recno = kingUnitRecno;

	Unit* kingUnit = unit_array[king_unit_recno];

	//--- if this unit currently has not have leadership ---//

	kingUnit->set_rank(RANK_KING);
	kingUnit->stop_order(); 		// clear the existing order, as there might be an assigning to firm/town order. But kings cannot be assigned to towns or firms as workers.

	//---------- king related vars ----------//

	char* kingName = nation_array.get_custom_king_name(nation_recno);
								 
	//-- if the custom king name hasn't been set or the first king has died and now there is a new king succeeding the throne, set a new custom king name --//

	if( !kingName || !firstKing )
	{
		char* kingName;

		if( kingUnit->is_human() )
			kingName = race_res[kingUnit->race_id]->get_name(kingUnit->name_id);
		else
			kingName = monster_res.get_name(kingUnit->name_id);

		nation_array.set_custom_king_name(nation_recno, kingName);
	}

	//----------- set nation name -----------//

	if( firstKing )
	{
		char* nationName = nation_array.get_custom_nation_name(nation_recno);

		if( !nationName[0] )
		{
//			String str;
//			str  = nation_array.get_custom_king_name(nation_recno);
//			if(kingUnit->race_id > 0)
//				str += "'s Kingdom";
//			else
//				str += "'s Kwyzan";
//			nation_array.set_custom_nation_name(nation_recno, str);

			// ########## begin Gilbert 26/6 ########//
			// use only the first word
			char *nameStr = nation_array.get_custom_king_name(nation_recno);
			char firstWord[Config.PLAYER_NAME_LEN+1];
			int i = 0;
			for( ; *nameStr == ' '; ++nameStr );	// trim leading space
			for( i = 0; *nameStr && *nameStr != ' ' && i < Config.PLAYER_NAME_LEN; ++i, ++nameStr )
				firstWord[i] = *nameStr;
			firstWord[i] = '\0';
			// ########## end Gilbert 26/6 ########//
			nation_array.set_custom_nation_name(nation_recno,
				text_unit.str_unit_nation( kingUnit->race_id, firstWord) );
		}
	}

	//-----------------------------------------//

	race_id 			 = kingUnit->race_id;
	king_leadership = kingUnit->skill_level();

	err_when( !race_id );
}
//----------- End of function NationBase::set_king ---------//


//--------- Begin of function NationBase::nation_name --------//
//
char* NationBase::nation_name()
{
	//---- if this nation has a custom name ------//

	char* nationName = nation_array.get_custom_nation_name(nation_recno);

	if( nationName[0] )
		return nationName;

	//-------------------------------------------------//

//#if(defined(SPANISH))
//	strncpy( nation_name_str, "Reino de ", NATION_NAME_LEN );
//	nation_name_str[NATION_NAME_LEN]=NULL;
//	strncat( nation_name_str, king_name(1), NATION_NAME_LEN );		// 1-get the first word of the name only
//	nation_name_str[NATION_NAME_LEN]=NULL;
//#elif(defined(FRENCH))
//	strncpy( nation_name_str, "Royaume de ", NATION_NAME_LEN );
//	nation_name_str[NATION_NAME_LEN]=NULL;
//	strncat( nation_name_str, king_name(1), NATION_NAME_LEN );		// 1-get the first word of the name only
//	nation_name_str[NATION_NAME_LEN]=NULL;
//#else
//	// German and US
//	strncpy( nation_name_str, king_name(1), NATION_NAME_LEN );		// 1-get the first word of the name only
//	nation_name_str[NATION_NAME_LEN]=NULL;
//	strncat( nation_name_str, "'s ", NATION_NAME_LEN );		// 1-get the first word of the name only
//	nation_name_str[NATION_NAME_LEN]=NULL;
//	strncat( nation_name_str, translate.process("Kingdom"), NATION_NAME_LEN );
//	nation_name_str[NATION_NAME_LEN]=NULL;
//#endif

	strncpy( nation_name_str, 
		text_unit.str_unit_nation(race_id, king_name(1)), NATION_NAME_LEN );	// 1-get the first word of the name only
	nation_name_str[NATION_NAME_LEN]=NULL;

	return nation_name_str;		// each name needs to have its own var as multiple nation names will be displayed at the same time in diplomatic talk choices
}
//----------- End of function NationBase::nation_name ---------//


//--------- Begin of function NationBase::king_name --------//
//
// [int] firstWordOnly - whether only get the first word of the name.
//								 (default: 0)
//
char* NationBase::king_name(int firstWordOnly)
{
	char* kingName = nation_array.get_custom_king_name(nation_recno, firstWordOnly);

	if( kingName )
		return kingName;

	//--------------------------------//

	if( king_unit_recno )
	{
		Unit* unitPtr = unit_array[king_unit_recno];

		if( unitPtr->is_human() )
			return race_res[unitPtr->race_id]->get_name(unitPtr->name_id, firstWordOnly?1:0);
		else
			return monster_res.get_name(unitPtr->name_id);
	}

	return "";
}
//----------- End of function NationBase::king_name ---------//


//------- Begin of function NationBase::cash_str ------//
//
// Return a text string with the current nation treasure and profit
// in the past 30 days.
//
char* NationBase::cash_str()
{
	static String str;

	if( cash >= 0 )
	{
		str = m.format( (int)cash, 4 );			// format type 4 - no thousand separators
	}
	else
	{
		str  = "-";
		str += m.format( (int)-cash, 4 );		// format type 4 - no thousand separators
	}

	//--------------------------------------//

	int curProfit = (int) profit_365days();

	if( curProfit )
	{
		str += " (";

		if( curProfit > 0 )
			str += "+";
		else
			str += "-";

		str += m.format( abs(curProfit), 4 ); 	// format type 4 - no thousand separators
		str += ")";
	}

	return str;
}
//------- End of function NationBase::cash_str -------//
//------- Begin of function NationBase::total_cash_str ------//
//
// Return a text string with the current nation treasure and profit
// in the past 30 days.
//
char* NationBase::total_cash_str()
{
	static String str;

	if( cash >= 0 )
	{
		str = m.format( (int)cash, 4 );			// format type 4 - no thousand separators
	}
	else
	{
		str  = "-";
		str += m.format( (int)-cash, 4 );		// format type 4 - no thousand separators
	}
	return str;
}
//------- End of function NationBase::total_cash_str -------//

//------- Begin of function NationBase::annual_cash_str ------//
//
// Return a text string with the current nation treasure and profit
// in the past 30 days.
//
char* NationBase::annual_cash_str()
{
	static String str;
	int curProfit = (int) profit_365days();
	if( curProfit )
	{
		str = " (";

		if( curProfit > 0 )
			str += "+";
		else
			str += "-";

		str += m.format( abs(curProfit), 4 ); 	// format type 4 - no thousand separators
		str += ")";
	}
	return str;
}
//------- End of function NationBase::annual_cash_str -------//


//------- Begin of function NationBase::food_str ------//
//
// Return a text string with the current nation treasure and profit
// in the past 30 days.
//
char* NationBase::food_str()
{
	static String str;

	if( food >= 0 )
	{
		str = m.format( (int)food, 4 );			// format type 4 - no thousand separators
	}
	else
	{
		str  = "-";
		str += m.format( (int)-food, 4 );		// format type 4 - no thousand separators
	}

	//--------------------------------------//

	int foodChange = (int) food_change_365days();

	if( foodChange )
	{
		str += " (";

		if( foodChange > 0 )
			str += "+";
		else
			str += "-";

		str += m.format( abs(foodChange), 4 ); 	// format type 4 - no thousand separators
		str += ")";
	}

	return str;
}
//------- End of function NationBase::food_str -------//
//------- Begin of function NationBase::total_food_str ------//
//
// Return a text string with the current nation treasure and profit
// in the past 30 days.
//
char* NationBase::total_food_str()
{
	static String str;

	if( food >= 0 )
	{
		str = m.format( (int)food, 4 );			// format type 4 - no thousand separators
	}
	else
	{
		str  = "-";
		str += m.format( (int)-food, 4 );		// format type 4 - no thousand separators
	}
	return str;
}
//------- End of function NationBase::total_food_str -------//

//------- Begin of function NationBase::annual_food_str ------//
//
// Return a text string with the current nation treasure and profit
// in the past 30 days.
//
char* NationBase::annual_food_str()
{
	static String str;
	int foodChange = (int) food_change_365days();
	if( foodChange )
	{
		str = " (";

		if( foodChange > 0 )
			str += "+";
		else
			str += "-";

		str += m.format( abs(foodChange), 4 ); 	// format type 4 - no thousand separators
		str += ")";
	}
	return str;
}
//------- End of function NationBase::annual_food_str -------//


//---------- Begin of function NationBase::next_day --------//
//
// This function is called every day.
//
void NationBase::next_day()
{
	//------ post is at war flag -------/

	if( is_at_war_today )
		last_war_date = info.game_date;

	is_at_war_yesterday = is_at_war_today;
	is_at_war_today	  = 0;

	//--- if the king is dead, and now looking for a successor ---//

	if( !king_unit_recno )
	{
		if( info.game_date%3 == nation_recno%3 )		// decrease 1 loyalty point every 3 days
			change_all_people_loyalty(-1);
	}

	//---------- debug code ------------//

#ifdef DEBUG

	err_when( is_human() && live_points != 0 );

	//---------- check king -----------//
	
	if( king_unit_recno )
	{
		Unit* unitPtr = unit_array[king_unit_recno];

		err_when( unitPtr->rank_id != RANK_KING );
		err_when( unitPtr->nation_recno != nation_recno );
	}

	//---------- check relation -----------//

	err_when( get_relation_status(nation_recno) != RELATION_ALLIANCE );		// relation to own nation must be alliance

	//------- validate attack_camp_array --------//

	for( int i=((Nation*)this)->attack_camp_count-1 ; i>=0 ; i-- )
	{
		firm_array[ ((Nation*)this)->attack_camp_array[i].firm_recno ];		// error if it is deleted
	}

#endif

	//---------- debug code --------------//
/*
	#ifdef DEBUG

	int totalHumanCount1=0, totalHumanCount2=0, totalMonsterCount=0;

	for( int i=unit_array.size() ; i>0 ; i-- )
	{
		Unit* unitPtr = (Unit*) unit_array.get_ptr(i);

		if( unitPtr &&
			 unitPtr->nation_recno == nation_recno &&
			 unitPtr->race_id &&
			 unitPtr->rank_id != RANK_KING )
		{
			if( unitPtr->is_human() )
			{
				if( unitPtr->unit_id == UNIT_WAGON )
					totalHumanCount1 += unitPtr->cast_to_UnitWagon()->population;
				else
					totalHumanCount1++;
			}
			else
				totalMonsterCount++;
		}
	}

	for( i=firm_array.size() ; i>0 ; i-- )
	{
		if( firm_array.is_deleted(i) )
			continue;

		Firm* firmPtr = firm_array[i];

		if( firmPtr->nation_recno != nation_recno )
			continue;

		if( firmPtr->cast_to_FirmCamp() )
		{
			FirmCamp *firmCamp = firmPtr->cast_to_FirmCamp();

			for( int j=firmCamp->soldier_count-1 ; j>=0 ; j-- )
			{
				if( firmCamp->soldier_array[j].is_human() )
					totalHumanCount2++;

				else if( firmCamp->soldier_array[j].is_monster() )
					totalMonsterCount++;
			}
		}

		if( firmPtr->cast_to_FirmTrain() )
		{
			FirmTrain *firmTrain = firmPtr->cast_to_FirmTrain();

			totalHumanCount2 += firmTrain->trainee_count;
		}
	}

	err_when( total_human_count != totalHumanCount1 + totalHumanCount2 );
//	err_when( total_monster_count != totalMonsterCount );

	#endif
*/
	//------ check if this nation has won the game -----//

	// ######## begin Gilbert 11/3 ######//
	// don't check win here, instead nationArray process

	// check_win();

	//if( !king_unit_recno )
	//	check_lose();
	// ###### end Gilbert 11/3 #######//
}
//----------- End of function NationBase::next_day ---------//


//---------- Begin of function NationBase::next_month --------//
//
// This function is called every month.
//
void NationBase::next_month()
{
	//--------------------------------------------------//
	// When the two nations, whose relationship is Tense,
	// do not have new conflicts for 3 years, their
	// relationship automatically becomes Neutral.
	//--------------------------------------------------//

	NationRelation* nationRelation;

	for( int i=1 ; i<=nation_array.size() ; i++ )
	{
		if( nation_array.is_deleted(i) )
			continue;

		nationRelation = get_relation(i);

		if( nationRelation->status == RELATION_TENSE &&
			 info.game_date >= nationRelation->last_change_status_date + 365*3 )
		{
			set_relation_status(i, RELATION_NEUTRAL);
		}

		//--- update good_relation_duration_rating ---//

		else if( nationRelation->status == RELATION_FRIENDLY )
			nationRelation->good_relation_duration_rating += (float) 0.2;		// this is the monthly increase

		else if( nationRelation->status == RELATION_ALLIANCE )
			nationRelation->good_relation_duration_rating += (float) 0.4;
	}

	//----- increase reputation gradually -----//

	// ###### patch begin Gilbert 29/3 #######//
	if( reputation < 100 )
		change_reputation((float)0.5);
	// 15 points per year if viking FIRM_BASE
	if( reputation < 15 )
		change_reputation( god_res[GOD_VIKING]->nation_prayer_count(nation_recno)*(15.0/12/MAX_WORKER) );
	// ###### patch end Gilbert 29/3 #######//
}
//---------- End of function NationBase::next_month --------//


//---------- Begin of function NationBase::next_year --------//
//
// This function is called every year
//
void NationBase::next_year()
{
	//------ post financial data --------//

	last_year_income = cur_year_income;
	cur_year_income  = (float) 0;

	last_year_expense = cur_year_expense;
	cur_year_expense  = (float) 0;

	last_year_fixed_income = cur_year_fixed_income;
	cur_year_fixed_income  = (float) 0;

	last_year_fixed_expense = cur_year_fixed_expense;
	cur_year_fixed_expense  = (float) 0;

	last_year_profit = cur_year_profit;
	cur_year_profit  = (float) 0;

	last_year_cheat = cur_year_cheat;
	cur_year_cheat  = (float) 0;

	//------ post income & expense breakdown ------//

	for( int i=0 ; i<INCOME_TYPE_COUNT ; i++ )
	{
		last_year_income_array[i] = cur_year_income_array[i];
		cur_year_income_array[i]  = (float) 0;
	}

	for( i=0 ; i<EXPENSE_TYPE_COUNT ; i++ )
	{
		last_year_expense_array[i] = cur_year_expense_array[i];
		cur_year_expense_array[i]  = (float) 0;
	}

	//------ post good change data ------//

	last_year_food_in = cur_year_food_in;
	cur_year_food_in  = (float) 0;

	last_year_food_out = cur_year_food_out;
	cur_year_food_out  = (float) 0;

	last_year_food_change = cur_year_food_change;
	cur_year_food_change  = (float) 0;

	//---------- post imports ----------//

	NationRelation* nationRelation = relation_array;

	for( i=0; i<MAX_NATION ; i++, nationRelation++ )
	{
		for( int j=0 ; j<IMPORT_TYPE_COUNT ; j++ )
		{
			nationRelation->last_year_import[j] = nationRelation->cur_year_import[j];
			nationRelation->cur_year_import[j] = (float) 0;
		}
	}

	//--------- post reputation & live points ----------//

	last_year_reputation_change = cur_year_reputation_change;
	cur_year_reputation_change  = (float) 0;

	last_year_live_points_change = cur_year_live_points_change;
	cur_year_live_points_change  = (float) 0;
}
//---------- End of function NationBase::next_year --------//


//---------- Begin of function NationBase::add_income --------//
//
// <int>   incomeType  - the income type
// <float> incomeAmt   - the amount of the income.
// [int]   fixedIncome - whether this is a fixed income, or
//								 a variable income. (default: 0)
//
void NationBase::add_income(int incomeType, float incomeAmt, int fixedIncome)
{
	err_when( incomeType < 0 || incomeType >= INCOME_TYPE_COUNT );

	cash += incomeAmt;

	cur_year_income_array[incomeType] += incomeAmt;

	cur_year_income += incomeAmt;
	cur_year_profit += incomeAmt;

	if( fixedIncome )
		cur_year_fixed_income += incomeAmt;
}
//---------- End of function NationBase::add_income --------//


//---------- Begin of function NationBase::add_expense --------//
//
// <int>   expenseType  - the expense type
// <float> incomeAmt    - the amount of the income.
// <int>   fixedExpense - whether this is a fixed expense, or
//								  a variable income. (default: 0)
//
void NationBase::add_expense(int expenseType, float expenseAmt, int fixedExpense)
{
	err_when( expenseType < 0 || expenseType >= EXPENSE_TYPE_COUNT );

	cash -= expenseAmt;

	cur_year_expense_array[expenseType] += expenseAmt;

	cur_year_expense += expenseAmt;
	cur_year_profit  -= expenseAmt;

	if( fixedExpense )
		cur_year_fixed_expense += expenseAmt;
}
//---------- End of function NationBase::add_expense --------//


//---------- Begin of function NationBase::add_cheat --------//
//
// <float> cheatAmount - the cheating amount.
//
void NationBase::add_cheat(float cheatAmount)
{
//	if( sys.testing_session || nation_type == NATION_OWN )
	if( sys.testing_session || nation_type != NATION_AI )
	{
		add_income(INCOME_CHEAT, cheatAmount);
	}
	else		// cheat less obviously, randomly add to one of the account at a random amount //
	{
		int itemId = m.random(INCOME_TYPE_COUNT-1)+1; // 1 - INCOME_CHEAT-1
		if( cheatAmount < 0 )
		{
			int selItemId = itemId;
			do
			{
				if( cur_year_income_array[selItemId] >= -cheatAmount )
				{
					itemId = selItemId;
					break;
				}
				if( ++itemId >= INCOME_TYPE_COUNT )		// exclude INCOME_CHEAT
					itemId = 0;
			} while( selItemId != itemId );

			if( cur_year_income_array[itemId] < -cheatAmount )
				return;	// no item to reduce
		}

		add_income( itemId, cheatAmount );
		cur_year_cheat += cheatAmount;
	}
}
//---------- End of function NationBase::add_cheat --------//


//---------- Begin of function NationBase::import_goods --------//
//
// This function is called when the current nation imports
// goods from a specific nation.
//
// <int> importType - type of goods imported.
//
// <int> importNationRecno - recno of the nation from which this
//								     nation imports.
//
// <float> importAmt 		- the import amount
//
void NationBase::import_goods(int importType, int importNationRecno, float importAmt)
{
	if( importNationRecno == nation_recno )
		return;

	NationRelation* nationRelation = get_relation(importNationRecno);

	nationRelation->cur_year_import[importType]   += importAmt;
	nationRelation->cur_year_import[IMPORT_TOTAL] += importAmt;

	add_expense(EXPENSE_IMPORTS, importAmt, 1);
	nation_array[importNationRecno]->add_income(INCOME_EXPORTS, importAmt, 1);
}
//---------- End of function NationBase::import_goods --------//


//---------- Begin of function NationBase::add_food --------//
//
// <float> foodToAdd - quantity of food to be added
//
void NationBase::add_food(float foodToAdd)
{
	food += foodToAdd;

	cur_year_food_in     += foodToAdd;
	cur_year_food_change += foodToAdd;
}
//----------- End of function NationBase::add_food ---------//


//---------- Begin of function NationBase::consume_food --------//
//
// <float> foodConsumed - quantity of food consumed
//
void NationBase::consume_food(float foodConsumed)
{
	food -= foodConsumed;

	cur_year_food_out    += foodConsumed;
	cur_year_food_change -= foodConsumed;
}
//----------- End of function NationBase::consume_food ---------//


//---------- Begin of function NationBase::change_reputation --------//
//
// <float> changeLevel - level of reputation to be changed.
//
void NationBase::change_reputation(float changeLevel)
{
	//--- reputation increase more slowly when it is close to 100 ----//

	if( changeLevel > 0 && reputation > 0 )
		changeLevel = changeLevel * (150-reputation) / 150;

	//-----------------------------------------------//

	reputation += changeLevel;

	if( reputation > 100 )
		reputation = (float) 100;

	if( reputation < -100 )
		reputation = (float) -100;

	//------ update cur_year_reputation_change ------//

	cur_year_reputation_change += changeLevel;
}
//---------- End of function NationBase::change_reputation --------//


//---------- Begin of function NationBase::change_live_points --------//
//
// <float> changeAmt - the amount of live_points to be changed.
//
void NationBase::change_live_points(float changeAmt)
{
	err_when( changeAmt != 0 && is_human() );		// human kingdom has no live points

	live_points += changeAmt;

	cur_year_live_points_change += changeAmt;

	err_when( live_points < 0 );
}
//---------- End of function NationBase::change_live_points --------//


//---------- Begin of function NationBase::change_ai_relation_level --------//
//
// Change this AI nation's subjectively relation levels towards the others,
// the opposite nation's relation level is not the same as this.
//
// ai_relation_level is for AI only and will only be reviewed by AI functions.
//
// <short> nationRecno   - the nation recno of the relation
// <int>   levelChange   - the amount of change to the relation level
//
void NationBase::change_ai_relation_level(short nationRecno, int levelChange)
{
	NationRelation* nationRelation = get_relation(nationRecno);

	int newLevel = nationRelation->ai_relation_level + levelChange;

	newLevel = min(newLevel, 100);
	newLevel = max(newLevel, 0  );

	nationRelation->ai_relation_level = newLevel;
}
//----------- End of function NationBase::change_ai_relation_level -----------//


//---------- Begin of function NationBase::set_relation_status --------//
//
// <short> nationRecno   - the nation recno of the relation
// <char>  newStatus     - the new relationship status
// [char]  recursiveCall - whether this is a recursive call from itself
//                      	(default: 0)
//
void NationBase::set_relation_status(short nationRecno, char newStatus, char recursiveCall)
{
	if( nationRecno == nation_recno ) 		// cannot set relation to itself
		return;

	NationRelation* nationRelation = get_relation(nationRecno);

	//--------- debug code --------// BUGHERE
/*
	#ifdef DEBUG
	if( nation_array[nationRecno]->is_ai() && is_ai() )	// AI must terminate the treaty first before declaring war
	{
		err_when( nationRelation->status >= RELATION_FRIENDLY &&
					 newStatus == RELATION_HOSTILE );
	}
	#endif
*/
	//-------------------------------------------------//
	//
	// When two nations agree to a cease-fire, there may
	// still be some bullets on their ways, and those
	// will set the status back to War status, so we need
	// the following code to handle this case.
	//
	//-------------------------------------------------//

	if( !recursiveCall &&
		 nationRelation->status == RELATION_TENSE &&
		 newStatus == RELATION_HOSTILE &&
		 info.game_date < nationRelation->last_change_status_date + 5 )		// 5 days after the cease-fire, the nation will remain cease-fire
	{
		return;
	}

	//-------------------------------------------------//
	//
	// If the nation cease fire or form a friendly/alliance
	// treaty with a nation. And this nation current
	// has plan to attack that nation, then cancel the plan.
	//
	//-------------------------------------------------//

	if( is_ai() )
	{
		if( newStatus==RELATION_TENSE ||		// cease fire
			 newStatus>=RELATION_FRIENDLY )		// new friendly/alliance treaty
		{
			if( ((Nation*)this)->ai_attack_target_nation_recno == nationRecno )
				((Nation*)this)->reset_ai_attack_target();
		}
	}

	//------------------------------------------------//

	relation_status_array[nationRecno-1] = newStatus;
	nationRelation->status = newStatus;
	nationRelation->last_change_status_date = info.game_date;

	int newRelationLevel = newStatus * RELATION_LEVEL_PER_STATUS;

	if( newRelationLevel < nationRelation->ai_relation_level )		// only set it when the new value is lower than the current value
		nationRelation->ai_relation_level = newRelationLevel;

	set_relation_passable(nationRecno, RELATION_FRIENDLY);

	//---------- set should_attack -------//

	if( newStatus==RELATION_ALLIANCE || newStatus==RELATION_FRIENDLY ||
		 newStatus==RELATION_TENSE )
	{
		set_relation_should_attack(nationRecno, 0, COMMAND_AUTO);
	}
	else if( newStatus==RELATION_HOSTILE )
	{
		set_relation_should_attack(nationRecno, 1, COMMAND_AUTO);
	}

	//----- share the nation contact with each other -----//
/*
		// these segment code will cause multiplayer sync problem

	if( newStatus == RELATION_ALLIANCE )
	{
		Nation* withNation = nation_array[nationRecno];

		for( i=nation_array.size() ; i>0 ; i-- )
		{
			if( i==nation_recno || i==nationRecno )
				continue;

			if( nation_array.is_deleted(i) )
				continue;

			//-- if we have contact with this nation and our ally doesn't, share the contact with it --//

			if( get_relation(i)->has_contact &&
				 withNation->get_relation(i)->has_contact==0 )
			{
				withNation->establish_contact(i);
			}
		}
	}
*/
	//--- if this is a call from a client function, not a recursive call ---//

	if( !recursiveCall )
	{
		nation_array[nationRecno]->set_relation_status(nation_recno, newStatus, 1);

		//-- auto terminate their trade treaty if two nations go into a war --//

		if( newStatus == RELATION_HOSTILE )
			set_trade_treaty(nationRecno, 0);
	}
}
//----------- End of function NationBase::set_relation_status -----------//


//---------- Begin of function NationBase::get_relation_status --------//

char NationBase::get_relation_status(short nationRecno)
{
	return relation_status_array[nationRecno-1];
}
//----------- End of function NationBase::get_relation_status -----------//


//---------- Begin of function NationBase::set_relation_passable --------//

void NationBase::set_relation_passable(short nationRecno, char status)
{
	relation_passable_array[nationRecno-1] = (relation_status_array[nationRecno-1] >= status);
}
//----------- End of function NationBase::set_relation_passable -----------//


//---------- Begin of function NationBase::get_relation_passable --------//

char NationBase::get_relation_passable(short nationRecno)
{
	return relation_passable_array[nationRecno-1];
}
//----------- End of function NationBase::get_relation_passable -----------//

#ifdef DEBUG

//---------- Begin of function NationBase::get_relation --------//
//
NationRelation* NationBase::get_relation(int nationRecno)
{
	if( nationRecno<1 || nationRecno>MAX_NATION )
		err.run( "NationBase::get_relation()" );

	return relation_array+nationRecno-1;
}
//---------- End of function NationBase::get_relation --------//

#endif

//---------- Begin of function NationBase::set_relation_should_attack --------//

void NationBase::set_relation_should_attack(short nationRecno, char newValue, char remoteAction)
{
	if( !remoteAction && remote.is_enable() )
	{
		short *shortPtr = (short *) remote.new_send_queue_msg(MSG_NATION_SET_SHOULD_ATTACK, 3*sizeof(short));
		*shortPtr = nation_recno;
		shortPtr[1] = nationRecno;
		shortPtr[2] = newValue;
	}
	else
	{
		relation_should_attack_array[nationRecno-1] = newValue;
		get_relation(nationRecno)->should_attack    = newValue;
	}
}
//---------- End of function NationBase::set_relation_should_attack --------//


//---------- Begin of function NationBase::get_relation_should_attack --------//

char NationBase::get_relation_should_attack(short nationRecno)
{
	// ###### begin Gilbert 3/9 #######//
	// always can attack independent unit
	return nationRecno == 0 || relation_should_attack_array[nationRecno-1];
	// ###### end Gilbert 3/9 #######//
}
//---------- End of function NationBase::get_relation_should_attack --------//


//------- Begin of function NationBase::form_friendly_treaty ------//
//
// <int> nationRecno - recno of the nation with which this nation
//							  will form treaty.
//
void NationBase::form_friendly_treaty(int nationRecno)
{
	err_when( nationRecno == nation_recno );
	err_when( nation_array.is_deleted(nationRecno) );

	set_relation_status(nationRecno, RELATION_FRIENDLY);
}
//------- End of function NationBase::form_friendly_treaty -------//


//------- Begin of function NationBase::form_alliance_treaty ------//
//
// <int> nationRecno - recno of the nation with which this nation
//							  will form treaty.
//
void NationBase::form_alliance_treaty(int nationRecno)
{
	err_when( nationRecno == nation_recno );
	err_when( nation_array.is_deleted(nationRecno) );

	set_relation_status(nationRecno, RELATION_ALLIANCE);

	//--- allied nations are oblied to trade with each other ---//

	set_trade_treaty(nationRecno, 1);

	//------ set is_allied_with_player -------//

	if( nationRecno == nation_array.player_recno )
		is_allied_with_player = 1;

	if( nation_recno == nation_array.player_recno )
		nation_array[nationRecno]->is_allied_with_player = 1;
}
//------- End of function NationBase::form_alliance_treaty -------//


//------- Begin of function NationBase::end_treaty ------//
//
// <int> withNationRecno - recno of the nation which ends treaty
//							  			  with this nation. If it's an alliance treaty,
//							  			  allied nations with break treaty with
//							  			  this nation.
//
// <int> newStatus - the new status after breaking the treaty.
//
void NationBase::end_treaty(int withNationRecno,int newStatus)
{
	//----- decrease reputation when terminating a treaty -----//

	Nation* withNation = nation_array[withNationRecno];

	if( withNation->reputation > 0 )
	{
		int curStatus = get_relation_status(withNationRecno);

		if( curStatus == TALK_END_FRIENDLY_TREATY )
			change_reputation( -withNation->reputation * 10 / 100 );
		else
			change_reputation( -withNation->reputation * 20 / 100 );
	}

	//------- reset good_relation_duration_rating -----//

	if( newStatus <= RELATION_NEUTRAL )
	{
		get_relation(withNationRecno)->good_relation_duration_rating = (float) 0;
		withNation->get_relation(nation_recno)->good_relation_duration_rating = (float) 0;
	}

	//------- set new relation status --------//

	set_relation_status(withNationRecno, newStatus);

	//------ set is_allied_with_player -------//

	if( withNationRecno == nation_array.player_recno )
		is_allied_with_player = 0;

	if( nation_recno == nation_array.player_recno )
		nation_array[withNationRecno]->is_allied_with_player = 0;
}
//------- End of function NationBase::end_treaty -------//


//------- Begin of function NationBase::set_trade_treaty ------//
//
// <int> nationRecno - recno of the nation to change the trade allow flag
//
void NationBase::set_trade_treaty(int nationRecno, char treatyFlag)
{
	err_when( nation_recno==nationRecno );		// cannot set trade treaty with oneself

	get_relation(nationRecno)->trade_treaty = treatyFlag;

	nation_array[nationRecno]->get_relation(nation_recno)->trade_treaty = treatyFlag;
}
//------- End of function NationBase::set_trade_treaty -------//


//------- Begin of function NationBase::establish_contact ------//
//
// <int> nationRecno - recno of the nation to establish contact with 
//
void NationBase::establish_contact(int nationRecno)
{
	get_relation(nationRecno)->has_contact = 1;
	nation_array[nationRecno]->get_relation(nation_recno)->has_contact = 1;
}
//------- End of function NationBase::establish_contact -------//


//------- Begin of function NationBase::being_attacked ------//
//
// <BaseObj*> attackerObj - BaseObj of the attacker object.
//
void NationBase::being_attacked(BaseObj* attackerObj)
{
	if( !attackerObj )
		return;

	last_attacker_obj_recno = attackerObj->base_obj_recno;

	int attackNationRecno = attackerObj->nation_recno;

	if( nation_array.is_deleted(attackNationRecno) ||
		 attackNationRecno == nation_recno )
	{
		return;
	}

	//--- if it is an accidential attack (e.g. bullets attack with spreading damages) ---//

	Nation* attackNation = nation_array[attackNationRecno];

	if( attackNation->get_relation(nation_recno)->should_attack==0 )
		return;

	//--- check if there a treaty between these two nations ---//

	NationRelation* nationRelation = get_relation(attackNationRecno);

	if( nationRelation->status != RELATION_HOSTILE )
	{
		//--- if this nation (the one being attacked) has a higher than 0 reputation, the attacker's reputation will decrease ---//

		if( reputation > 0 )
			attackNation->change_reputation( -reputation * 40 / 100 );

		nationRelation->started_war_on_us_count++;	// how many times this nation has started a war with us, the more the times the worse this nation is.

		if( nationRelation->status == RELATION_ALLIANCE ||
			 nationRelation->status == RELATION_FRIENDLY )
		{
			attackNation->end_treaty(nation_recno, RELATION_HOSTILE);		// the attacking nation abruptly terminates the treaty with us, not we terminate the treaty with them, so attackNation->end_treaty() should be called instead of end_treaty()
		}
		else
		{
			set_relation_status(attackNationRecno, RELATION_HOSTILE);
		}
	}

	//---- reset the inter-national peace days counter ----//

	nation_array.nation_peace_days = 0;
}
//------- End of function NationBase::being_attacked -------//


//------- Begin of function NationBase::disp_nation_color ------//
//
// Display the color of the nation in a rectangular box.
//
void NationBase::disp_nation_color(int x, int y)
{
	vga.active_buf->draw_d3_up_border( x, y-2, x+12, y+10 );
	vga.active_buf->bar( x+3, y+1, x+9, y+7, nation_color );
}
//------- End of function NationBase::disp_nation_color -------//


//------- Begin of function NationBase::trade_rating ------//
//
// Return a rating from 0 to 100 telling the significance of
// trading with the specific nation in this nation's regard.
//
int NationBase::trade_rating(int nationRecno)
{
	// use an absolute value 5000 as the divider.

	int tradeRating1 = 100 * (int) total_year_trade(nationRecno) / 5000;

	int tradeRating2 = 50 * (int) nation_array[nationRecno]->get_relation(nation_recno)->last_year_import[IMPORT_TOTAL] / (int) (last_year_income+1) +
							 50 * (int) get_relation(nationRecno)->last_year_import[IMPORT_TOTAL] / (int) (last_year_expense+1);

	return max(tradeRating1, tradeRating2);
}
//------- End of function NationBase::trade_rating -------//


//------- Begin of function NationBase::revealed_by_phoenix ------//
//
// Whether the area is revealed by phoenix of the nation or not.
//
// <int> xLoc, yLoc - the location
//
int NationBase::revealed_by_phoenix(int xLoc, int yLoc)
{
	err_here();		// should call Nation::revealed_by_phoenix instead

	// BUGHERE : UNIT_PHOENIX is removed
	/*
	Unit* unitPtr;

	int 	effectiveRange = unit_res[UNIT_PHOENIX]->visual_range;

	for( int i=unit_array.size() ; i>0 ; i-- )
	{
		if( unit_array.is_deleted(i) )
			continue;

		unitPtr = unit_array[i];

		if( unitPtr->unit_id == UNIT_PHOENIX &&
			 unitPtr->nation_recno == nation_recno )
		{
			if( m.points_distance( xLoc, yLoc,
				 unitPtr->next_x_loc(), unitPtr->next_y_loc() ) <= effectiveRange )
			{
				return 1;
			}
		}
	}
	*/

	return 0;
}
//------- End of function NationBase::revealed_by_phoenix -------//


//------- Begin of function NationBase::total_tech_level -------//
//
// The sum of the tech levels of weapons that this nation possesses.
//
// [int] unitClass - only calculate total tech levels of this
//						   unit class. (default: 0, any classes)
//
int NationBase::total_tech_level(int unitClass)
{
	TechInfo* techInfo;
	int techLevel, totalTechLevel=0;

	for( int i=1 ; i<=tech_res.tech_count ; i++ )
	{
		techInfo  = tech_res[i];
		techLevel = techInfo->get_nation_tech_level(nation_recno);

		// ####### begin Gilbert 23/6 ########//
		if( techLevel>0 )
		{
			if( !unitClass ||
				 (techInfo->unit_id && unit_res[techInfo->unit_id]->unit_class == unitClass) )
			{
				totalTechLevel += techLevel;
			}
		}
		// ####### end Gilbert 23/6 ########//
	}

	return totalTechLevel;
}
//-------- End of function NationBase::total_tech_level -------//


//---------- Begin of function NationBase::civilian_killed --------//
//
// This function is called when civilian units are killed and this
// nation either killed the civilian units or this nation's civilian
// units are killed.
//
// <int> civilianRaceId - the race id. of the civilian unit killed
//								  0 - all races, when a Caravan is killed, 0 will
//								  be passed, the loyalty of all races will be decreased.
//
// <int> isAttacker     - 1 if the nation is the offensive attacker.
//								  0 if the nation that suffers the atttack.
//
// [int] killedCount	- the number of civilian units killed.
//							  this can be higher than 1 for wagons.
//							  (default: 1)
//
void NationBase::civilian_killed(int civilianRaceId, int isAttacker, int killedCount)
{
	if( isAttacker )
	{
		change_all_people_loyalty((float)-2 * killedCount, civilianRaceId);

		if( civilianRaceId==0 )				// a caravan
			change_reputation((float)-10 * killedCount);
		else
			change_reputation((float)-1 * killedCount);	// a mobile civilian unit, a citizen in town, or a wagon
	}
/*
	else
	{
		change_all_people_loyalty((float)-0.6 * killedCount, civilianRaceId);

		if( civilianRaceId==0 )				// a caravan
			change_reputation(-(float)2 * killedCount);
		else
			change_reputation(-(float)0.3 * killedCount);
	}
*/
}
//----------- End of function NationBase::civilian_killed ---------//


//--------- Begin of function NationBase::succeed_king --------//
//
// Appoint a unit to succeed the king who has died.
//
// The loyalty of the people in the natino may change due
// to the succession.
//
void NationBase::succeed_king(int kingUnitRecno)
{
	Unit* newKing = unit_array[kingUnitRecno];

	err_when( !newKing->can_succeed_king() );
	err_when( newKing->combat_level() <= 0 );
	err_when( !unit_array[kingUnitRecno]->can_succeed_king() );

	///------------------------------------------//

	int newKingLeadership=0;

	newKingLeadership = newKing->skill_level();

	newKingLeadership = max( 20, newKingLeadership );		// give the king a minimum level of leadership

	//----- set the common loyalty change for all races ------//

	int loyaltyChange=0;

	if( newKingLeadership < king_leadership )
		loyaltyChange = (newKingLeadership-king_leadership)/3;

	if( newKing->rank_id != RANK_GENERAL )
		loyaltyChange -= 20;

	//---- update loyalty of units in this nation ----//

	Unit* unitPtr;

	for( int i=unit_array.size() ; i>0 ; i-- )
	{
		if( unit_array.is_deleted(i) )
			continue;

		if( i==king_unit_recno || i==kingUnitRecno )
			continue;

		unitPtr = unit_array[i];

		if( unitPtr->nation_recno != nation_recno )
			continue;

		//--------- update loyalty change ----------//

		unitPtr->change_loyalty( loyaltyChange +
			succeed_king_loyalty_change(unitPtr->race_id, newKing->race_id, race_id) );
	}

	//---- update loyalty of units in camps ----//

	Firm* firmPtr;

	for( i=firm_array.size() ; i>0 ; i-- )
	{
		if( firm_array.is_deleted(i) )
			continue;

		firmPtr = firm_array[i];

		if( firmPtr->nation_recno != nation_recno )
			continue;

		//------ process military camps and seat of power -------//

		if( firmPtr->cast_to_FirmCamp() )
		{
			FirmCamp *firmCamp = firmPtr->cast_to_FirmCamp();
			Soldier* soldierPtr = firmCamp->soldier_array;

			for(int j=firmCamp->soldier_count-1 ; j>=0 ; j--, soldierPtr++ )
			{
				//--------- update loyalty change ----------//

				soldierPtr->change_loyalty( loyaltyChange +
					succeed_king_loyalty_change(soldierPtr->race_id, newKing->race_id, race_id ) );
			}
		}
	}

	//---- update loyalty of town people ----//

	Town* townPtr;

	for( i=town_array.size() ; i>0 ; i-- )
	{
		if( town_array.is_deleted(i) )
			continue;

		townPtr = town_array[i];

		if( townPtr->nation_recno != nation_recno )
			continue;

		if( townPtr->population > 0 )
		{
			townPtr->change_loyalty( (float) loyaltyChange +
				succeed_king_loyalty_change( townPtr->race_id, newKing->race_id, race_id ) );
		}
	}

	//--- remove all messages using the old king's name as only one king's name can be stored at a time ---//

	news_array.remove(NEWS_KING_DIE, -1, nation_recno);
	// ####### begin Gilbert 19/2 #########//
	news_array.remove(NEWS_UNIT_ASSASSINATED, RANK_KING, nation_recno );
	// ####### end Gilbert 19/2 #########//

	//------- add news --------//

	news_array.new_king(nation_recno, kingUnitRecno);

	//-------- set the new king now ------//

	set_king(kingUnitRecno, 0);		// 0-not the first king, it is a succession

	//------ if the new king is a spy -------//

	if( newKing->spy_recno )
	{
		Spy* spyPtr = spy_array[newKing->spy_recno];

		if( newKing->true_nation_recno() == nation_recno )      // if this is your spy
			spyPtr->drop_spy_identity();
		else
			spyPtr->think_become_king();

		err_when( newKing->rank_id==RANK_KING && newKing->spy_recno );		// it can't still be a spy when it asscends as the king
	}
}
//----------- End of function NationBase::succeed_king ---------//


//------ Begin of static function succeed_king_loyalty_change -----//
//
// Return the amount of loyalty should be changed.
//
static int succeed_king_loyalty_change(int thisRaceId, int newKingRaceId, int oldKingRaceId)
{
	#define SAME_RACE_LOYALTY_INC          20
	#define DIFFERENT_RACE_LOYALTY_DEC	   30

	//----- the races of the new and old kings are different ----//

	if( newKingRaceId != oldKingRaceId )
	{
		//--- if this unit's race is the same as the new king ---//

		if( thisRaceId == newKingRaceId )
			return SAME_RACE_LOYALTY_INC;

		//--- if this unit's race is the same as the old king ---//

		else if( thisRaceId == oldKingRaceId )
			return DIFFERENT_RACE_LOYALTY_DEC;
	}

	return 0;
}
//-------- End of static function succeed_king_loyalty_change ------//


//----- Begin of function NationBase::has_king_successor_candidate ------//
//
// Whether whether this nation has any eligible candidate for succeeding
// the king.
//
int NationBase::has_king_successor_candidate()
{
	for( int i=unit_array.size() ; i>0 ; i-- )
	{
		if( unit_array.is_deleted(i) )
			continue;

		Unit* unitPtr = unit_array[i];

		if( unitPtr->nation_recno == nation_recno &&
			 unitPtr->can_succeed_king() &&
			 unitPtr->rank_id != RANK_KING &&
			 !unitPtr->is_civilian() )
		{
			return 1;
		}
	}

	for( i=firm_array.size() ; i>0 ; i-- )
	{
		if( firm_array.is_deleted(i) )
			continue;

		Firm* firmPtr = firm_array[i];

		if( firmPtr->nation_recno != nation_recno )
			continue;

		if( firmPtr->cast_to_FirmCamp() )
		{
			FirmCamp *firmCamp = firmPtr->cast_to_FirmCamp();

			for( int j=firmCamp->soldier_count-1 ; j>=0 ; j-- )
			{
				if( firmCamp->soldier_array[j].can_succeed_king(nation_recno) )
					return 1;
			}
		}
	}

	return 0;
}
//------- End of function NationBase::has_king_successor_candidate ------//


//--------- Begin of function NationBase::surrender --------//
//
// This nation surrenders.
//
// <int> toNationRecno - the recno of the nation this nation
//								 surrenders to.
//
void NationBase::surrender(int toNationRecno)
{
	news_array.nation_surrender(nation_recno, toNationRecno);

	//---- the king demote himself to General first ----//

	if( king_unit_recno )
	{
		unit_array[king_unit_recno]->set_rank(RANK_GENERAL);
  		king_unit_recno = 0;
	}

	//------- if the player surrenders --------//

	if( nation_recno==nation_array.player_recno )
		game.game_end(0, 1, toNationRecno);

	//--- hand over the entire nation to another nation ---//

	hand_over_to(toNationRecno);
}
//----------- End of function NationBase::surrender ---------//


//--------- Begin of function NationBase::defeated --------//
//
// This nation is defeated.
//
void NationBase::defeated()
{
	//---- if the defeated nation is the player's nation ----//

	if( nation_recno == nation_array.player_recno )
	{
		game.game_end(0, 1);		// the player lost the game 
	}
	else	// AI and remote players 
	{
		news_array.nation_destroyed(nation_recno);
	}

	//---- delete this nation from nation_array ----//

	nation_array.del_nation(nation_recno);
}
//----------- End of function NationBase::defeated ---------//


//------- Begin of function NationBase::change_all_people_loyalty ------//
//
// Change the loyalty of all the people in your nation.
//
// <float> loyaltyChange - degree of loyalty change
// [int] raceId		  - if this is given, then only people of this race
//								 will be affected. (default: 0)
//
void NationBase::change_all_people_loyalty(float loyaltyChange, int raceId)
{
	//---- update loyalty of units in this nation ----//

	Unit* unitPtr;

	for( int i=unit_array.size() ; i>0 ; i-- )
	{
		if( unit_array.is_deleted(i) )
			continue;

		if( i==king_unit_recno )
			continue;

		unitPtr = unit_array[i];

		if( unitPtr->nation_recno != nation_recno )
			continue;

		//--------- update loyalty change ----------//

		if( !raceId || unitPtr->race_id == raceId )
			unitPtr->change_loyalty((int)loyaltyChange);
	}

	//---- update loyalty of units in camps ----//

	Firm* firmPtr;

	for( i=firm_array.size() ; i>0 ; i-- )
	{
		if( firm_array.is_deleted(i) )
			continue;

		firmPtr = firm_array[i];

		if( firmPtr->nation_recno != nation_recno )
			continue;

		//------ process military camps and seat of power -------//

		if( firmPtr->cast_to_FirmCamp() )
		{
			FirmCamp *firmCamp = firmPtr->cast_to_FirmCamp();

			Soldier* soldierPtr = firmCamp->soldier_array;

			for(int j=firmCamp->soldier_count-1 ; j>=0 ; j--, soldierPtr++ )
			{
				if( !raceId || soldierPtr->race_id == raceId )
					soldierPtr->change_loyalty((int)loyaltyChange);
			}
		}
	}

	//---- update loyalty of town people ----//

	Town* townPtr;

	for( i=town_array.size() ; i>0 ; i-- )
	{
		if( town_array.is_deleted(i) )
			continue;

		townPtr = town_array[i];

		if( townPtr->nation_recno != nation_recno )
			continue;

		//--------------------------------------//

		if( raceId )		// decrease loyalty of a specific race
		{
			if( townPtr->race_id == raceId && townPtr->population > 0 )
				townPtr->change_loyalty(loyaltyChange);
		}
		else					// decrease loyalty of all races
		{
			if( townPtr->population > 0 )
				townPtr->change_loyalty(loyaltyChange);
		}
	}
}
//------- End of function NationBase::change_all_people_loyalty -------//


//------- Begin of function NationBase::total_year_trade ------//
//
// Return the total trade (import + export) with the specific
// trade in the last year.
//
float NationBase::total_year_trade(int nationRecno)
{
	return get_relation(nationRecno)->last_year_import[IMPORT_TOTAL] +
			 nation_array[nationRecno]->get_relation(nation_recno)->last_year_import[IMPORT_TOTAL];
}
//------- End of function NationBase::total_year_trade -------//


//---------- Begin of function NationBase::check_win --------//
//
// Check if the player has won the game.
//
// [int] checkOnlyFlag : if true, do not call game.game_end (called by game.game_end)
//
int NationBase::check_win(int checkOnlyFlag)
{
	int i, justFinishedTutorial;
	//--- if the player only wins when all the required conditions are met ---//

	if( !checkOnlyFlag && game.game_mode == GAME_TUTORIAL )
	{
		justFinishedTutorial = tutor.update_tutorial_objective_finish_count();
		if (justFinishedTutorial >= 0)
		{
			// in tutorial mode and have just finished an objective
			tutor.disp_result_text(justFinishedTutorial);
			for( i=0; i< MAX_TUTOR_OBJECTIVE; i++ )
			{
				if (tutor.achieved_objective_array[i] == 0)
				{
					tutor.disp_objective_text(i);
					break;
				}
			}
		}
	}

	if( game.win_require_all_condition )
	{
		for( i=0 ; i<game.win_condition_count ; i++ )
		{
			if( !game.win_condition[i].is_condition_true(nation_recno) )
				return 0;
		}
		if( !checkOnlyFlag )
		{
			if( game.game_mode == GAME_TUTORIAL )
				tutor.update_tutorial_finish_count();
			game.result_win_condition_id = 0; // set it to the first winning condition
			game.result_win_condition_recno = 0;
			game.game_end(nation_recno);
		}
		return 1;
	}
	else //--- if the player wins when any of the conditions is met ----//
	{
		for( i=0 ; i<game.win_condition_count ; i++ )
		{
			if( game.win_condition[i].is_condition_true(nation_recno) )
			{
				if( !checkOnlyFlag )
				{
					if( game.game_mode == GAME_TUTORIAL )
						tutor.update_tutorial_finish_count();
					game.result_win_condition_id = game.win_condition[i].condition_id;
					game.result_win_condition_recno = i;
					game.game_end(nation_recno);
				}
				return 1;
			}
		}

		return 0;
	}
}
//----------- End of function NationBase::check_win ---------//


//---------- Begin of function NationBase::check_lose --------//
//
// Check if the player has lost the game.
//
// If the player still hasn't selected a unit to succeed the
// died king, declare defeated if the all units are killed.
//
int NationBase::check_lose(int checkOnlyFlag)
{
	//---- if this nation has been defeated -----//

	if( !king_unit_recno && !has_king_successor_candidate() )
	{
		if( !checkOnlyFlag )
		{
			if( nation_type == NATION_OWN )
				game.result_defeated = 1;

			defeated();
		}
		return 1;
	}

	//---- if one of the losing conditions is true for the player ----//

	if( nation_type == NATION_OWN || checkOnlyFlag )	// save check time
	{
		for( int i=0 ; i<game.lose_condition_count ; i++ )
		{
			if( game.lose_condition[i].is_condition_true(nation_recno) )
			{
				if( nation_type == NATION_OWN && !checkOnlyFlag )
				{
					game.result_lose_condition_id = game.lose_condition[i].condition_id;
					game.result_lose_condition_recno = i;
					game.game_end(0);
				}
				return 1;
			}
		}
	}
	return 0;
}
//----------- End of function NationBase::check_lose ---------//


//---------- Begin of function NationBase::give_tribute --------//
//
// Give tribute or aid to a nation.
//
// <int> toNationRecno - give tribute to this nation
// <int> tributeAmt    - the amount of the tribute
//
void NationBase::give_tribute(int toNationRecno, int tributeAmt)
{
	Nation* toNation = nation_array[toNationRecno];

	add_expense( EXPENSE_TRIBUTE, (float) tributeAmt );

	toNation->add_income( INCOME_TRIBUTE, (float) tributeAmt );

	NationRelation* nationRelation = get_relation(toNationRecno);

	nationRelation->last_give_gift_date      = info.game_date;
	nationRelation->total_given_gift_amount += tributeAmt;

	//---- set the last rejected date so it won't request or give again soon ----//

	nationRelation->set_never_accept_until_date(TALK_GIVE_AID, 180);
	nationRelation->set_never_accept_until_date(TALK_DEMAND_AID, 180);
	nationRelation->set_never_accept_until_date(TALK_GIVE_TRIBUTE, 180);
	nationRelation->set_never_accept_until_date(TALK_DEMAND_TRIBUTE, 180);

	NationRelation* nationRelation2 = toNation->get_relation(nation_recno);

	nationRelation2->set_never_accept_until_date(TALK_GIVE_AID, 180);
	nationRelation2->set_never_accept_until_date(TALK_DEMAND_AID, 180);
	nationRelation2->set_never_accept_until_date(TALK_GIVE_TRIBUTE, 180);
	nationRelation2->set_never_accept_until_date(TALK_DEMAND_TRIBUTE, 180);
}
//----------- End of function NationBase::give_tribute ---------//


//---------- Begin of function NationBase::give_tech --------//
//
// Give tribute or aid to a nation.
//
// <int> toNationRecno - give tribute to this nation
// <int> techId        - id. of the technology
// <int> techVersion   - version of the technology
//
void NationBase::give_tech(int toNationRecno, int techId, int techVersion)
{
	Nation* toNation = nation_array[toNationRecno];

	int curVersion = tech_res[techId]->get_nation_tech_level(toNationRecno);

	if( curVersion < techVersion )
		tech_res[techId]->set_nation_tech_level( toNationRecno, techVersion );
}
//----------- End of function NationBase::give_tech ---------//


//----- Begin of function NationBase::base_town_count_in_region -----//
//
// Return the number of base towns in the given region.
//
int NationBase::base_town_count_in_region(int regionId)
{
	// ###### patch begin Gilbert 16/3 #######//
	// regionStatId may be zero
	int regionStatId = region_array[regionId]->region_stat_id;
	if( regionStatId )
	{
		return region_array.get_region_stat2(regionStatId)->
			 base_town_nation_count_array[nation_recno-1];
	}
	else if( region_array[regionId]->region_size < STD_TOWN_LOC_WIDTH*STD_TOWN_LOC_HEIGHT)
	{
		return 0;			// not enough to build any town
	}
	else
	{
		int townCount = 0;
		for( int townRecno = town_array.size(); townRecno > 0; --townRecno )
		{
			if( town_array.is_deleted(townRecno) )
				continue;

			Town *townPtr = town_array[townRecno];
			if( townPtr->region_id == regionId && townPtr->nation_recno == nation_recno )
				townCount++;
		}
		return townCount == 0;
	}
	// ###### patch end Gilbert 16/3 #######//
}
//------ End of function NationBase::base_town_count_in_region -----//


//----- Begin of function NationBase::peace_duration_str -----//
//
char* NationBase::peace_duration_str()
{
	int peaceDays  = peaceful_days();
	int peaceYear  = peaceDays / 365;
	int peaceMonth = (peaceDays - peaceYear * 365) / 30;

//	static String str;
//	str = "";
//	if( peaceYear > 0 )
//	{
//		str += peaceYear;
//		str += translate.process( peaceYear>1 ? " years" : " year" );
//		str += translate.process( " and " );
//	}
//	str += peaceMonth;
//	str += translate.process( peaceMonth>1  ? " months" : " month" );
//	return str;

	return text_reports.str_duration_ym( peaceYear, peaceMonth );
}
//------ End of function NationBase::peace_duration_str -----//


//----- Begin of function NationBase::true_income_365days -----//
//
// Return the total income of the nation, excluding cheats.
//
float NationBase::true_income_365days()
{
	float curYearIncome=(float)0, lastYearIncome=(float)0;

	for( int i=0 ; i<INCOME_TYPE_COUNT-1 ; i++ )		// -1 to exclude cheat
	{
		curYearIncome  += cur_year_income_array[i];
		lastYearIncome += last_year_income_array[i];
	}

	return lastYearIncome * (365-info.year_day) / 365 + curYearIncome;
}
//------ End of function NationBase::true_income_365days -----//


//----- Begin of function NationBase::update_nation_rating -----//
//
void NationBase::update_nation_rating()
{
	population_rating = get_population_rating();

	economic_rating = get_economic_rating();

	overall_rating  = get_overall_rating();
}
//------ End of function NationBase::update_nation_rating -----//


//----- Begin of function NationBase::get_population_rating -----//
//
int NationBase::get_population_rating()
{
	return all_population();
}
//------ End of function NationBase::get_population_rating -----//


//----- Begin of function NationBase::get_economic_rating -----//
//
int NationBase::get_economic_rating()
{
	return (int) cash / 300 +
			 (int) true_income_365days()/2 +
			 (int) true_profit_365days();
}
//------ End of function NationBase::get_economic_rating -----//


//----- Begin of function NationBase::get_overall_rating -----//
//
int NationBase::get_overall_rating()
{
	//--- the calculation of overall score is different between human and Fryhtans ---//

	if( is_human() )
	{
		return  33*population_rating/500+
				  33*military_rating/200+
				  33*economic_rating/10000;
	}
	else
	{
		return  10*population_rating/500+
				  45*military_rating/200+
				  45*economic_rating/10000;
	}
}
//------ End of function NationBase::get_overall_rating -----//


//----- Begin of function NationBase::population_rank_rating -----//
//
int NationBase::population_rank_rating()
{
	if( nation_array.max_population_rating==0 )
		return 0;

	return 100 * population_rating / nation_array.max_population_rating;
}
//------ End of function NationBase::population_rank_rating -----//


//----- Begin of function NationBase::military_rank_rating -----//
//
int NationBase::military_rank_rating()
{
	if( nation_array.max_military_rating==0 )
		return 0;

	return 100 * military_rating / nation_array.max_military_rating;
}
//------ End of function NationBase::military_rank_rating -----//


//----- Begin of function NationBase::economic_rank_rating -----//
//
int NationBase::economic_rank_rating()
{
	if( nation_array.max_economic_rating==0 )
		return 0;

	return 100 * economic_rating / nation_array.max_economic_rating;
}
//------ End of function NationBase::economic_rank_rating -----//


//----- Begin of function NationBase::reputation_rank_rating -----//
//
int NationBase::reputation_rank_rating()
{
	if( nation_array.max_reputation==0 )
		return 0;

	return 100 * (int) reputation / nation_array.max_reputation;
}
//------ End of function NationBase::reputation_rank_rating -----//


//----- Begin of function NationBase::kill_monster_rank_rating -----//
//
int NationBase::kill_monster_rank_rating()
{
	if( nation_array.max_kill_monster_score==0 )
		return 0;

	if( config.monster_type == OPTION_MONSTER_NONE )
		return 0;

	return 100 * (int) kill_monster_score / nation_array.max_kill_monster_score;
}
//------ End of function NationBase::kill_monster_rank_rating -----//


//----- Begin of function NationBase::overall_rank_rating -----//
//
int NationBase::overall_rank_rating()
{
	if( nation_array.max_overall_rating==0 )
		return 0;

	return 100 * overall_rating / nation_array.max_overall_rating;
}
//------ End of function NationBase::overall_rank_rating -----//


//------- Begin of function NationBase::goal_destroy_nation_achieved --------//

int NationBase::goal_destroy_nation_achieved()
{
	return nation_array.nation_count==1;
}
//------- End of function NationBase::goal_destroy_nation_achieved --------//


//------- Begin of function NationBase::goal_destroy_monster_achieved --------//

int NationBase::goal_destroy_monster_achieved()
{
	if( !config.goal_destroy_monster )		// this is not one of the required goals.
		return 0;

	if( config.monster_type == OPTION_MONSTER_NONE )
		return 0;

	//------- when all monsters have been killed -------//
/*
	if( firm_res[FIRM_MONSTER]->total_firm_count == 0 &&
		 unit_res.mobile_monster_count == 0 )
	{
		Nation* nationPtr;
		float	  maxScore=(float)0;

		for( int i=nation_array.size() ; i>0 ; i-- )
		{
			if( nation_array.is_deleted(i) )
				continue;

			nationPtr = nation_array[i];

			if( nationPtr->kill_monster_score > maxScore )
				maxScore = nationPtr->kill_monster_score;
		}

		//-- if this nation is the one that has destroyed most monsters, it wins, otherwise it loses --//

		return maxScore == kill_monster_score;
	}
*/
	return 0;
}
//------- End of function NationBase::goal_destroy_monster_achieved --------//


//------- Begin of function NationBase::goal_population_achieved --------//

int NationBase::goal_population_achieved()
{
	if( !config.goal_population_flag )		// this is not one of the required goals.
		return 0;

	return all_population() >= config.goal_population;
}
//------- End of function NationBase::goal_population_achieved --------//


//------ Begin of function NationBase::goal_economic_score_achieved ------//

int NationBase::goal_economic_score_achieved()
{
	if( !config.goal_economic_score_flag )
		return 0;

	info.set_rank_data(0);		// 0-set all nations, not just those that have contact with us

	return info.get_rank_score(3,nation_recno) >= config.goal_economic_score;
}
//------- End of function NationBase::goal_economic_score_achieved -------//


//------ Begin of function NationBase::goal_total_score_achieved -------//

int NationBase::goal_total_score_achieved()
{
	if( !config.goal_total_score_flag )
		return 0;

	info.set_rank_data(0);	   // 0-set all nations, not just those that have contact with us

	return info.get_total_score(nation_recno) >= config.goal_total_score;
}
//------- End of function NationBase::goal_total_score_achieved --------//


//----- Begin of function NationBase::set_auto_collect_tax_loyalty -----//
//
void NationBase::set_auto_collect_tax_loyalty(int loyaltyLevel)
{
	auto_collect_tax_loyalty = loyaltyLevel;

	if( auto_collect_tax_loyalty > 0 &&
		 auto_grant_loyalty > 0 &&
		 auto_grant_loyalty >= auto_collect_tax_loyalty )
	{
		auto_grant_loyalty = auto_collect_tax_loyalty-10;
	}

	//------ set all towns of this nation ------//

	for( int townRecno=town_array.size() ; townRecno>0 ; townRecno-- )
	{
		if( town_array.is_deleted(townRecno) )
			continue;

		Town *townPtr = town_array[townRecno];

		if( townPtr->nation_recno == nation_recno )
		{
			townPtr->set_auto_collect_tax_loyalty(loyaltyLevel);

			if( town_array.selected_recno == townRecno )
				info.disp();
		}
	}
}
//------ End of function NationBase::set_auto_collect_tax_loyalty -----//


//----- Begin of function NationBase::set_auto_grant_loyalty -----//
//
void NationBase::set_auto_grant_loyalty(int loyaltyLevel)
{
	auto_grant_loyalty = loyaltyLevel;

	if( auto_collect_tax_loyalty > 0 &&
		 auto_grant_loyalty > 0 &&
		 auto_grant_loyalty >= auto_collect_tax_loyalty )
	{
		auto_collect_tax_loyalty = auto_grant_loyalty+10;

		if( auto_collect_tax_loyalty > 100 )
			auto_collect_tax_loyalty = 0;					// disable auto collect tax if it's over 100
	}

	//------ set all towns of this nation ------//

	for( int townRecno=town_array.size() ; townRecno>0 ; townRecno-- )
	{
		if( town_array.is_deleted(townRecno) )
			continue;

		Town *townPtr = town_array[townRecno];

		if( townPtr->nation_recno == nation_recno )
		{
			townPtr->set_auto_grant_loyalty(loyaltyLevel);

			if( town_array.selected_recno == townRecno )
				info.disp();
		}
	}
}
//------ End of function NationBase::set_auto_grant_loyalty -----//


// ####### begin Gilbert 21/4 #########//
//------ Begin of function NationBase::repair_all_building -----//
//
void NationBase::repair_all_building(int flag, char remoteAction )
{
	if( !remoteAction && remote.is_enable() )
	{
		// packet structure : <nation recno> <repair flag>
		short *shortPtr = (short *)remote.new_send_queue_msg( MSG_NATION_REPAIR_ALL_BUILDING, 2*sizeof(short) );
		shortPtr[0] = nation_recno;
		shortPtr[1] = flag;
		return;
	}

	//------ set all towns of this nation ------//

	for( int townRecno=town_array.size() ; townRecno>0 ; townRecno-- )
	{
		if( town_array.is_deleted(townRecno) )
			continue;

		Town *townPtr = town_array[townRecno];

		if( townPtr->nation_recno != nation_recno )
			continue;

		if( flag && !townPtr->repair_flag 
			&& townPtr->current_wall_level > 0 && townPtr->hit_points < townPtr->max_hit_points()
			|| !flag && townPtr->repair_flag )
		{
			townPtr->set_repair_wall( flag, COMMAND_AUTO );
		}
	}

	//------ set all firms of this nation ------//

	for( int firmRecno=firm_array.size() ; firmRecno>0 ; firmRecno-- )
	{
		if( firm_array.is_deleted(firmRecno) )
			continue;

		Firm *firmPtr = firm_array[firmRecno];

		if( firmPtr->nation_recno != nation_recno )
			continue;

		if( flag && !firmPtr->repair_flag
			&& firmPtr->hit_points < firmPtr->max_hit_points()
			|| !flag && firmPtr->repair_flag )
		{
			firmPtr->set_repair_flag( flag, COMMAND_AUTO );
		}
	}
}
//------ End of function NationBase::repair_all_building -----//
// ####### end Gilbert 21/4 #########//

//------ Begin of function NationBase::set_auto_retreat -----//

void NationBase::set_auto_retreat(int hitPointPercentage)
{
	auto_retreat_hit_point = hitPointPercentage;
		
	//------ set all towns of this nation ------//

	for( int unitRecno=unit_array.size() ; unitRecno>0 ; unitRecno-- )
	{
		if( unit_array.is_deleted(unitRecno) )
			continue;

		Unit *unitPtr = unit_array[unitRecno];

		if( unitPtr->nation_recno == nation_recno && unitPtr->home_camp_firm_recno 
			&& unitPtr->rank_id == RANK_GENERAL )
		{
			unitPtr->set_auto_retreat(hitPointPercentage);

			if( unit_array.selected_recno == unitRecno )
				info.disp();
		}
	}
}
//------ End of function NationBase::set_auto_retreat -----//


//----- Begin of function NationBase::reset_all_finance -----//
//
// For scenario creation and editing, reset all current year finances.
//
void NationBase::reset_all_finance()
{
	next_year();		// reset all finances this year and last year to zero.
	next_year();
}
//------ End of function NationBase::reset_all_finance -----//


//----- Begin of function NationRelation::status_duration_str -----//
//
char* NationRelation::status_duration_str()
{
	int statusDays  = info.game_date - last_change_status_date;
	int statusYear  = statusDays / 365;
	int statusMonth = (statusDays - statusYear * 365) / 30;

//	static String str;
//	str = "";
//	if( statusYear > 0 )
//	{
//		str += statusYear;
//		str += translate.process( statusYear>1 ? " years" : " year" );
//		str += translate.process( " and " );
//	}
//	str += statusMonth;
//	str += translate.process( statusMonth>1  ? " months" : " month" );
//	return str;

	return text_reports.str_duration_ym( statusYear, statusMonth );
}
//------ End of function NationRelation::status_duration_str -----//


//----- Begin of function NationRelation::set_ai_never_consider -----//
//
// Batch set ai_never_consider[] to 1.
//
// <short*> neverConsiderArray - the array containing talk message id.
//											for setting ai_never_consider[]. The
//											latest element of the array is 0,
//											indicating its end.
//
void NationRelation::set_ai_never_consider(short* neverConsiderArray)
{
	for( int i=0 ; neverConsiderArray[i] ; i++ )
	{
		int talkId = neverConsiderArray[i];

		ai_never_consider[talkId-1] = 1;

		err_when( i>=MAX_TALK_TYPE );
	}
}
//------ End of function NationRelation::set_ai_never_consider -----//


//----- Begin of function NationRelation::set_never_accept_until_date -----//
//
void NationRelation::set_never_accept_until_date(int talkId, int dayCount)
{
	int newDate = info.game_date + dayCount * (70 + m.random(60)) / 100;   // -30 to +30 random 

	never_accept_until_date_array[talkId-1] = max( never_accept_until_date_array[talkId-1], newDate );
}
//------ End of function NationRelation::set_never_accept_until_date -----//


//----- Begin of function NationRelation::reset_ai_never_consider -----//
//
// Batch reset ai_never_consider[] to 0.
//
// [short*] neverConsiderArray - the array containing talk message id.
//											for setting ai_never_consider[]. The
//											latest element of the array is 0,
//											indicating its end.
//
//											(If this is not given, all will be reset)
//
void NationRelation::reset_ai_never_consider(short* neverConsiderArray)
{
	if( !neverConsiderArray )
	{
		memset( ai_never_consider, 0, sizeof(ai_never_consider) );
	}
	else
	{
		for( int i=0 ; neverConsiderArray[i] ; i++ )
		{
			int talkId = neverConsiderArray[i];

			ai_never_consider[talkId-1] = 0;

			err_when( i>=MAX_TALK_TYPE );
		}
	}
}
//------ End of function NationRelation::reset_ai_never_consider -----//


//--------- Begin of function NationBase::start_research --------//
//
// Start researching a specific technology.
//
// <int> techId - id. of the technology to research.
//
void NationBase::start_research(int techId, char remoteAction)
{
	if( !remoteAction && remote.is_enable())
	{
		short *shortPtr = (short *) remote.new_send_queue_msg( MSG_NATION_RESEARCH, 2*sizeof(short) );
		// packet structure : <nation recno> <tech id>
		shortPtr[0] = nation_recno;
		shortPtr[1] = techId;
		return;
	}

	// ###### begin Gilbert 14/3 ######//
	if( research_tech_id != techId )
	{
		// reset the fractional part of research progress
		fraction_research_progress = 0.0f;
	}

	research_tech_id 	= techId;
	// ###### end Gilbert 14/3 ######//
}
//----------- End of function NationBase::start_research ---------//

//--------- Begin of function NationBase::progress_research --------//
//
// Add new progress to the current research.
//
// <float> newProgress - the amount of new progress.
//
// return: <int> 1 - if the current research is complete
//					  0 - still researching
//
int NationBase::progress_research(float newProgress)
{
	if( research_tech_id==0 )
		return 0;

	// ####### begin Gilbert 14/3 #########//

	newProgress += research_progress();		// newProgress is now the new research_progres

	if( newProgress >= 100 )
	{
		set_research_progress( (newProgress = 0.0f) );		// reset the progress

		int newTechLevel = tech_res[research_tech_id]->get_nation_tech_level(nation_recno)+1;

		//------ if this is the player nation ------//

		if( nation_type == NATION_OWN )
		{
			se_ctrl.immediate_sound("RESE_COM");
			news_array.tech_researched( research_tech_id, newTechLevel );
		}

		//---------------------------------------------//

		tech_res[research_tech_id]->set_nation_tech_level( nation_recno, newTechLevel );

		return 1;
	}

	set_research_progress(newProgress);

	// ####### end Gilbert 14/3 #########//

	return 0;
}
//----------- End of function NationBase::progress_research ---------//


// ---------- Begin of function NationBase::research_progress --------//
//
// percent completed on researching the current technology
//
// integral part of research progress stored in tech res
// and fractional part of the researching progress store in 
// nation's fraction_research_progress
//
float NationBase::research_progress()
{
	if( !research_tech_id )
		return 0.0f;

	return (float)tech_res[research_tech_id]->nation_research_progress_array[nation_recno-1] + fraction_research_progress;
}
// ---------- End of function NationBase::research_progress --------//


// ---------- Begin of function NationBase::set_research_progress --------//
//
// percent completed on researching the current technology
//
void NationBase::set_research_progress(float newProgress)
{
	if( !research_tech_id )
		return;

	if( newProgress > 127.0f )		// max limit of intProgress
		newProgress = 127.0f;
	char intProgress = (char) newProgress;

	tech_res[research_tech_id]->nation_research_progress_array[nation_recno-1] = intProgress;
	fraction_research_progress = newProgress - (float) intProgress;
}
// ---------- End of function NationBase::set_research_progress --------//


// ---------- begin of function NationRelation::status_str ----------//
//
char* NationRelation::status_str()
{
	return text_talk.str_relation_status( status );
}
// ---------- end of function NationRelation::status_str ----------//
