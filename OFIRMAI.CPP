//Filename    : OFIRMAI.CPP
//Description : AI functions for the class Firm

#include <OF_INN.H>
#include <OF_MINE.H>
#include <OF_FACT.H>
#include <OF_CAMP.H>
#include <OF_MARK.H>
#include <OUNITRES.H>
#include <OTALKRES.H>
#include <ONATION.H>

//--------- Begin of function Firm::process_common_ai --------//
//
// AI processing functions common for all firm types.
//
void Firm::process_common_ai()
{
	//------ think about closing this firm ------//

	if( !should_close_flag )
	{
		if( info.game_date%15 == firm_recno%15 )
		{
			if( ai_should_close() )
			{
				should_close_flag = 1;
				nation_array[nation_recno]->firm_should_close_array[firm_id-1]++;
			}
		}
	}

	//------ think about repairing the firm -------//

	// ####### begin Gilbert 30/3 #######//
	if( hit_points < max_hit_points() && !should_close_flag &&
		(!nation_recno || nation_ptr()->ai_should_spend(50 + nation_ptr()->pref_repair_concern/2)) )
	{
		set_repair_flag(1, COMMAND_AI);
	}
	// ####### end Gilbert 30/3 #######//
}
//--------- End of function Firm::process_common_ai --------//


//------- Begin of function Firm::ai_del_firm -----------//
//
// Delete the firm no matter what status this firm is in.
//
void Firm::ai_del_firm()
{
	if( under_construction )
	{
		cancel_construction(COMMAND_PLAYER);
	}
	else
	{
		if( can_sell() )
			sell_firm(COMMAND_AI);
		else
			destruct_firm(COMMAND_AI);
	}
}
//--------- End of function Firm::ai_del_firm -----------//


//------- Begin of function Firm::ai_should_close -----------//
//
// This is function is for derived class to overload.
//
int Firm::ai_should_close()
{
	return 0;
}
//--------- End of function Firm::ai_should_close -----------//


//------- Begin of function Firm::ai_build_neighbor_firm -----------//
//
int Firm::ai_build_neighbor_firm(int firmId)
{
	short   buildXLoc, buildYLoc;
	Nation* nationPtr = nation_array[nation_recno];

	if( !nationPtr->find_best_place_loc(firmId, loc_x1, loc_y1, buildXLoc, buildYLoc) )
	{
		no_neighbor_space = 1;
		return 0;
	}

	nationPtr->add_action(buildXLoc, buildYLoc, loc_x1, loc_y1, ACTION_AI_BUILD_FIRM, firmId);
	return 1;
}
//--------- End of function Firm::ai_build_neighbor_firm -----------//


//------- Begin of function Firm::think_build_factory -----------//
//
int Firm::think_build_factory(int rawId)
{
	if( no_neighbor_space )		// if there is no space in the neighbor area for building a new firm.
		return 0;

	Nation* nationPtr = nation_array[nation_recno];

	//--- check whether the AI can build a new firm next this firm ---//

	if( !nationPtr->can_ai_build(FIRM_FACTORY) )
		return 0;

	//---------------------------------------------------//

	int 			 factoryCount=0;
	FirmFactory* firmPtr;
	Firm*			 firmMarket;

	for(int i=0; i<linked_firm_count; i++)
	{
		err_when(!linked_firm_array[i] || firm_array.is_deleted(linked_firm_array[i]));

		firmPtr = (FirmFactory*) firm_array[linked_firm_array[i]];

		if(firmPtr->firm_id!=FIRM_FACTORY)
			continue;

		if( firmPtr->nation_recno != nation_recno )
			continue;

		if( firmPtr->product_raw_id != rawId )
			continue;

		//---- if the nation does not have enough workers, do not build a new factory ---//

		if( nationPtr->total_jobless_population <
			 MAX_WORKER * 2 * (50+nationPtr->pref_inc_pop_by_growth) / 100 )
		{
			return 0;
		}

		//--- if one of own factories still has not recruited enough workers ---//

		if( firmPtr->worker_count < MAX_WORKER )
			return 0;

		//---------------------------------------------------//
		//
		// If this factory has a medium to high level of stock,
		// this means the bottleneck is not at the factories,
		// building more factories won't solve the problem.
		//
		//---------------------------------------------------//

		if( firmPtr->stock_qty > firmPtr->max_stock_qty * 0.1 )
			return 0;

		//---------------------------------------------------//
		//
		// Check if this factory is just outputing goods to
		// a market and it is actually not overcapacity.
		//
		//---------------------------------------------------//

		for( int j=firmPtr->linked_firm_count-1 ; j>=0 ; j-- )
		{
			if( firmPtr->linked_firm_enable_array[j] != LINK_EE )
				continue;

			firmMarket = firm_array[ firmPtr->linked_firm_array[j] ];

			if( firmMarket->firm_id != FIRM_MARKET )
				continue;

			//--- if this factory is producing enough goods to the market place, then it means it is still quite efficient

			MarketGoods *marketGoods = firmMarket->cast_to_FirmMarket()->market_product_array(rawId);

			if( marketGoods && marketGoods->stock_qty > 100 )
				return 0;
		}

		//----------------------------------------------//

		factoryCount++;
	}

	//---- don't build additional factory if we don't have enough peasants ---//

	if( factoryCount>=1 && !nationPtr->ai_has_enough_food() )
		return 0;

	//-- if there isn't much raw reserve left, don't build new factories --//

	if( firm_id == FIRM_MINE )
	{
		if( ((FirmMine*)this)->reserve_qty < 1000 && factoryCount>=1 )
			return 0;
	}

	//--- only build additional factories if we have a surplus of labor ---//

	if( nationPtr->total_jobless_population < factoryCount * MAX_WORKER )
		return 0;

	//--- only when we have checked it three times and all say it needs a factory, we then build a factory ---//

	if( ++ai_should_build_factory_count >= 3 )
	{
		short buildXLoc, buildYLoc;

		if( !nationPtr->find_best_place_loc(FIRM_FACTORY, loc_x1, loc_y1, buildXLoc, buildYLoc) )
		{
			no_neighbor_space = 1;
			return 0;
		}

		nationPtr->add_action(buildXLoc, buildYLoc, loc_x1, loc_y1, ACTION_AI_BUILD_FIRM, FIRM_FACTORY);

		ai_should_build_factory_count = 0;
	}

	return 1;
}
//--------- End of function Firm::think_build_factory -----------//


//------- Begin of function Firm::think_linked_town_change_nation ------//
//
// This function is called by Town::set_nation() when a town linked
// to this firm has changed nation.
//
// <int> linkedTownRecno - the recno of the town that has changed nation.
// <int> oldNationRecno  - the old nation recno of the town
// <int> newNationRecno  - the new nation recno of the town
//
void Firm::think_linked_town_change_nation(int linkedTownRecno, int oldNationRecno, int newNationRecno)
{

}
//-------- End of function Firm::think_linked_town_change_nation ------//


//--------- Begin of function Firm::ai_firm_captured --------//
//
// This is function is called when the AI's firm is just
// about to be captured.
//
void Firm::ai_firm_captured(int capturerNationRecno)
{
	Nation* ownNation = nation_array[nation_recno];

	if( !ownNation->is_ai() )			//**BUGHERE
		return;

	if( ownNation->get_relation(capturerNationRecno)->status >= RELATION_FRIENDLY )
		ownNation->ai_end_treaty(capturerNationRecno);

	talk_res.ai_send_talk_msg(capturerNationRecno, nation_recno, TALK_DECLARE_WAR);
}
//--------- End of function Firm::ai_firm_captured --------//

