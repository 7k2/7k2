// Filename    : OF_BASEW.CPP
// Description : Seat of power wish


#include <OF_BASE.H>
#include <OREMOTE.H>
#include <OTECH.H>
#include <OTECHRES.H>
#include <ORACERES.H>
#include <OMONSRES.H>
#include <OUNITRES.H>
#include <OHERORES.H>
#include <OITEMRES.H>
#include <ONATION.H>
#include <OSITE.H>
#include <OUNIT.H>
#include <OSPY.H>
#include <OTOWN.H>
#include <OF_CAMP.H>
#include <OF_TRAIN.H>
#include <OT_FIRM.H>


enum
{
	WISH_FINISH_TECH = 1,
	WISH_NEW_TECH,
	WISH_NEW_HUMAN,
	WISH_NEW_MONSTER,
	WISH_NEW_SPY,
	WISH_NEW_HERO,
	WISH_ITEM,
	WISH_CASH,
	WISH_FOOD,
	WISH_RAW,
	WISH_CAPTURE_UNIT,
	WISH_CAPTURE_CAMP,
	WISH_CAPTURE_TOWN,
	WISH_KILL_SPY_BUILDING,
	WISH_KILL_SPY_NATION,
	WISH_CLEAR_SPY_NATION,
	WISH_HEAL_BUILDING,
	WISH_HEAL_NATION,
	WISH_LOYALTY_BUILDING_S,
	WISH_LOYALTY_BUILDING_L,
	WISH_LOYALTY_NATION_S,
	WISH_LOYALTY_NATION_L,
	WISH_COMBAT_BUILDING,

	WISH_LAST,		// KEEP these two last
	MAX_WISH = WISH_LAST-1
};


struct WishInfo
{
	short wish_id;
	short rare;
	char *desc;
};



WishInfo wish_info_array[MAX_WISH] = 
{  //id                    rare, desc
	{ WISH_FINISH_TECH,       90, "Technology breakthrough - complete the current research instantly" },
	{ WISH_NEW_TECH,          80, "Technology breakthrough - gain a new technology" },
	{ WISH_NEW_HUMAN,         90, "A veteran human unit comes to serve you" },
	{ WISH_NEW_MONSTER,       50, "A fryhtan unit comes to serve you" },
	{ WISH_NEW_SPY,           85, "A veteran spy unit comes to serve you" },
	{ WISH_NEW_HERO,          80, "A hero comes to serve you" },
	{ WISH_ITEM,              60, "An item" },
	{ WISH_CASH,              90, "Cash bonus" },
	{ WISH_FOOD,              90, "Food bonus" },
	{ WISH_RAW,               60, "Creation of a new mineral deposit and instantly building a mine on top of it." },
	{ WISH_CAPTURE_UNIT,      70, "Convert an enemy unit (except king) to your side" },
	{ WISH_CAPTURE_CAMP,      40, "Instantly capture an enemy fort or camp" },
	{ WISH_CAPTURE_TOWN,      40, "Instantly caputre an enemy town" },
	{ WISH_KILL_SPY_BUILDING, 60, "Reveal and kill a few enemy spies in a town or structures of yours" },
	{ WISH_KILL_SPY_NATION,   40, "Reveal and kill a few enemy spies in your kingdom" },
	{ WISH_CLEAR_SPY_NATION,  10, "Reveal and kill every enemy spy in your kingdom" },
	{ WISH_HEAL_BUILDING,     80, "Heal all of your wounded soldiers in a structures" },
	{ WISH_HEAL_NATION,       10, "Heal all of your wonded soldiers in your kingdom" },
	{ WISH_LOYALTY_BUILDING_S, 5, "Increase the loyalty of a selected town or the loyalty of all units in a structure by 20-50" },
	{ WISH_LOYALTY_BUILDING_L,10, "Increase the loyalty of a selected town or the loyalty of all units in a structure to 100" },
	{ WISH_LOYALTY_NATION_S,  10, "Increase the loyalty of everybody in your kingdom by 20-50" },
	{ WISH_LOYALTY_NATION_L,  20, "Increase the loyalty of everybody in your kingdom to 100" },
	{ WISH_COMBAT_BUILDING,   50, "Increase the combat level of all soldiers in a structure by 30-70" },
};


int FirmBase::can_make_wish()
{
	return pray_points >= (float)MAX_PRAY_POINTS;
}

void FirmBase::invoke_random_effect(int effectId, char remoteAction)
{
	if( !can_make_wish() )
		return;

	if( !remoteAction && remote.is_enable() )
	{
		// packet struct : <firm recno> <effect id>
		short *shortPtr = (short *)remote.new_send_queue_msg(MSG_F_BASE_INVOKE_EFFECT, 2*sizeof(short));
		shortPtr[0] = firm_recno;
		shortPtr[1] = effectId;
		return;
	}

	if( !effectId )
	{
		static long staticSumRare = 0;

		// find staticSumRare

		if( !staticSumRare )
		{
			for( effectId = 1; effectId <= MAX_WISH; ++effectId )
			{
				staticSumRare += wish_info_array[effectId-1].rare;
			}
			err_when( staticSumRare > 0x7fff );	// larger the limit of m.random
		}

		long sumRare = m.random(staticSumRare);
		for( effectId = 1; effectId <= MAX_WISH; ++effectId )
		{
			sumRare -= wish_info_array[effectId-1].rare;
			if( sumRare < 0 )
				break;
		}

		err_when( effectId > MAX_WISH );
	}

	if( !effectId )
		return;

	int viewX = -1;
	int viewY = -1;

	String resultStr = "";

	switch(effectId)
	{
	case WISH_FINISH_TECH:
	// Technology breakthrough! Your present research project has been completed!
		if( nation_ptr()->research_tech_id )
		{
			nation_ptr()->progress_research(100);
			resultStr = text_firm.get_format_str( 13, "BAW1");
		}
		break;

	case WISH_NEW_TECH:
	// Technology breakthrough! You have acquired <technology> technology!
		{
			short validTechIdCount = 0;
			short validTechId[MAX_TECH];
			short techId;
			for( techId = 1; techId <= MAX_TECH; ++techId )
			{
         	//##### begin trevor 4/1 ######//

				if( tech_res[techId]->can_research(nation_recno) &&
					 nation_ptr()->research_tech_id != techId )				// if the nation is currently researching this tech, don't consider it
				{
					validTechId[validTechIdCount++] = techId;
				}

				//##### end trevor 4/1 ######//
			}

			if( validTechIdCount > 0 )
			{
				techId = validTechId[m.random(validTechIdCount)];
				TechInfo *techInfo = tech_res[techId];
				techInfo->set_nation_tech_level(nation_recno, techInfo->get_nation_tech_level(nation_recno)+1 );

				resultStr = text_firm.process( text_firm.get_format_str( 14, "BAW2" ),
					techInfo->tech_des() );
			}
		}
		break;

	case WISH_NEW_HUMAN:
	// A veteran <race> soldier has entered your service!
	case WISH_NEW_SPY:
	// A veteran <race> spy has entered your service!
		{
			int unitId;
			Skill skill;
			int humanClass = 1+m.random(2);
			switch( humanClass )
			{
			case 0: 
				unitId = race_res[race_id]->civilian_unit_id;
				skill.init( unitId, CITIZEN_COMBAT_LEVEL, 0 );
				break;

			case 1:
				unitId = race_res[race_id]->infantry_unit_id;
				skill.init( unitId, MAX_COMBAT_TRAIN+m.random(MAX_COMBAT_BATTLE-MAX_COMBAT_TRAIN), BASIC_SKILL_TRAIN+m.random(MAX_SKILL_TRAIN-BASIC_SKILL_TRAIN) );
				break;

			case 2:
				unitId = race_res[race_id]->special_unit_id;
				skill.init( unitId, MAX_COMBAT_TRAIN+m.random(MAX_COMBAT_BATTLE-MAX_COMBAT_TRAIN), BASIC_SKILL_TRAIN+m.random(MAX_SKILL_TRAIN-BASIC_SKILL_TRAIN) );
				break;
			}

			int unitRecno = create_unit( unitId );
			if( unitRecno )
			{
				Unit *unitPtr = unit_array[unitRecno];
				unitPtr->skill = skill;
				unitPtr->loyalty = 100;
				unitPtr->init_attack_info();
				unitPtr->set_combat_level(-1);

				if( effectId == WISH_NEW_HUMAN )
				{
					resultStr = text_firm.process( text_firm.get_format_str( 15, "BAW3" ),
						race_res[unitPtr->race_id]->name );
				}
				else if( effectId == WISH_NEW_SPY )
				{
					unitPtr->spy_recno = spy_array.add_spy(unitPtr->sprite_recno, 100-m.random(20) );
					resultStr = text_firm.process( text_firm.get_format_str( 16, "BAW4" ),
						race_res[unitPtr->race_id]->name );
				}
				else
				{
					err_here();
				}

				viewX = unitPtr->next_x_loc();
				viewY = unitPtr->next_y_loc();
			}
		}
		break;

	case WISH_NEW_MONSTER:
	// A fryhtan has entered your service!
		{
			int monsterId = 1+m.random(MAX_MONSTER_TYPE);
			int unitId = monster_res[monsterId]->unit_id;
			Skill skill;
			// skill.init( unitId, BASIC_COMBAT_TRAIN, BASIC_SKILL_TRAIN );
			skill.init( unitId, BASIC_COMBAT_TRAIN+m.random(MAX_COMBAT_TRAIN-BASIC_COMBAT_TRAIN), BASIC_SKILL_TRAIN+m.random(MAX_SKILL_TRAIN-BASIC_SKILL_TRAIN) );

			int unitRecno = create_unit( unitId );
			if( unitRecno )
			{
				Unit *unitPtr = unit_array[unitRecno];
				unitPtr->skill = skill;
				unitPtr->loyalty = 100;
				unitPtr->init_attack_info();
				unitPtr->set_combat_level(-1);

				resultStr = text_firm.get_format_str( 17, "BAW5" );

				viewX = unitPtr->next_x_loc();
				viewY = unitPtr->next_y_loc();
			}
		}
		break;

	case WISH_NEW_HERO:
	// A Hero, <name>, has offered you his service!
		{
			int         xLoc=loc_x1, yLoc=loc_y1;     // xLoc & yLoc are used for returning results

			// assume locate width and height are bigger, size 2

			if(locate_space(is_being_deleted, xLoc, yLoc, loc_x2, loc_y2, 2, 2))
			{
				int unitRecno = hero_res.create_hero( xLoc, yLoc, nation_recno, race_id, 0, 0, 1);
				if( unitRecno )
				{
					Unit *unitPtr = unit_array[unitRecno];

					resultStr = text_firm.process( text_firm.get_format_str( 18, "BAW6" ),
						unitPtr->unit_name(0) );

					viewX = unit_array[unitRecno]->next_x_loc();
					viewY = unit_array[unitRecno]->next_y_loc();
				}
			}
		}
		break;

	case WISH_ITEM:
	// An artifact has been uncovered near here! Pick it up before it is discovered by your enemies
		{
			int         xLoc=loc_x1-2, yLoc=loc_y1-2;

			// assume locate width and height are bigger, size 2

			if(locate_space(is_being_deleted, xLoc, yLoc, loc_x2+2, loc_y2+2, 1, 1))
			{
				Item randomItem;
				randomItem.init_random(MAX_RARITY-1, MAX_RARITY);
				if( !randomItem.id )
					randomItem.init_random(MIN_RARITY, MAX_RARITY);
				if( randomItem.id )
				{
					site_array.add_site( xLoc, yLoc, SITE_ITEM, randomItem.id, randomItem.para );

					resultStr = text_firm.get_format_str( 19, "BAW7" );

					viewX = xLoc;
					viewY = yLoc;
				}
			}
		}
		break;

	case WISH_CASH:
		// A peasant has discovered a hidden hoard of gold has has gladly turned it all over to you!
		nation_ptr()->add_income( INCOME_TREASURE, (20+m.random(51)) * 100.0f );
		resultStr = text_firm.get_format_str( 20, "BAW8" );
		break;

	case WISH_FOOD:
		// A large amount of food has been discovered in a secret peasant cache.
		// They have recognized their crime and have turned the food over to you.
		nation_ptr()->add_food( (20+m.random(51)) * 100.0f );
		resultStr = text_firm.get_format_str( 21, "BAW9" );
		break;

	case WISH_RAW:
	// A deposit has been found and a new mine built on top of it!
		{
			// create site
			int siteRecno = site_array.create_raw_site( region_id );
			if( siteRecno )
			{
				// ###### begin Gilbert 29/6 #######//
				int newFirmRecno = firm_array.build_firm(
					site_array[siteRecno]->map_x_loc, site_array[siteRecno]->map_y_loc,
					nation_recno, FIRM_MINE, race_res[race_id]->code,		// nation_ptr->race_id may be negative
					0, false, true );	// no cost
				// ###### end Gilbert 29/6 #######//

				// BUGHERE : need to set it to complete?
				if( newFirmRecno )
				{
					Firm *newFirmPtr = firm_array[newFirmRecno];
					newFirmPtr->under_construction = 0;
					newFirmPtr->hit_points = (float) newFirmPtr->max_hit_points();

					resultStr = text_firm.get_format_str( 22, "BAWA" );

					viewX = newFirmPtr->center_x;
					viewY = newFirmPtr->center_y;
				}
			}
		}
		break;

	case WISH_CAPTURE_UNIT:
	// An enemy (soldier or civilian) has defected and joined your Kingdom!
		{
			int unitArraySize = unit_array.size();
			int unitRecno = m.random(unitArraySize);
			int i;
			for( i = unitArraySize; i > 0; --i )
			{
				if( ++unitRecno > unitArraySize )
					unitRecno = 1;

				if( unit_array.is_deleted(unitRecno) )
					continue;

				Unit *unitPtr = unit_array[unitRecno];

				if( unitPtr->rank_id != RANK_KING 
					&& unitPtr->is_visible()
					&& !unitPtr->is_nation(nation_recno)					
					&& !unitPtr->spy_recno
					&& unitPtr->race_id
					&& unitPtr->unit_id != UNIT_WAGON
					&& unitPtr->unit_mode != UNIT_MODE_REBEL )
				{
					unitPtr->betray( nation_recno );

					resultStr = text_firm.process( text_firm.get_format_str( 23, "BAWB" ),
						unitPtr->unit_name() );

					viewX = unitPtr->next_x_loc();
					viewY = unitPtr->next_y_loc();
					break;
				}
			}
		}
		break;

	case WISH_CAPTURE_CAMP:
	// An enemy <fort> has recognized your greatness and surrendered to you!
		{
			int firmArraySize = firm_array.size();
			int firmRecno = m.random(firmArraySize);
			int i;

			for( i = firmArraySize; i > 0; --i )
			{
				if( ++firmRecno > firmArraySize )
					firmRecno = 1;

				if( firm_array.is_deleted(firmRecno) )
					continue;

				Firm *firmPtr = firm_array[firmRecno];

				if( firmPtr->under_construction )
					continue;

				FirmCamp *camp;

				if( firmPtr->nation_recno != nation_recno 
					&& (camp = firmPtr->cast_to_FirmCamp()) && firmPtr->is_human()			// don't change monster lair
					&& (!camp->overseer_recno || unit_array[camp->overseer_recno]->rank_id != RANK_KING		// don't change camp with a king
					&& unit_array[camp->overseer_recno]->is_human()) )
				{
					if( camp->overseer_recno )
						unit_array[camp->overseer_recno]->betray( nation_recno );
					else
						firmPtr->change_nation( nation_recno );

					resultStr = text_firm.process( text_firm.get_format_str( 24, "BAWC" ),
						firm_res.get_build(firmPtr->firm_build_id)->name );

					viewX = firmPtr->center_x;
					viewY = firmPtr->center_y;
					break;
				}
			}
		}
		break;

	case WISH_CAPTURE_TOWN:
	// An enemy town, tired of living under oppressive rule, has declared for you!
		{
			int townArraySize = town_array.size();
			int townRecno = m.random(townArraySize);
			int i;

			for( i = townArraySize; i > 0; --i )
			{
				if( ++townRecno > townArraySize )
					townRecno = 1;

				if( town_array.is_deleted(townRecno) )
					continue;

				Town *townPtr = town_array[townRecno];

				if( townPtr->under_construction )
					continue;

				if( townPtr->nation_recno != nation_recno && (townPtr->surrender(nation_recno), townPtr->nation_recno == nation_recno) )
				{
					resultStr = text_firm.get_format_str( 25, "BAWD" );

					viewX = townPtr->center_x;
					viewY = townPtr->center_y;
					break;
				}
			}
		}
		break;

	case WISH_KILL_SPY_BUILDING:
	// <number> enemy spy has/spyies have been exposed and executed!
		{
			// discover any spy first
			int spyArraySize = spy_array.size();
			int spyRecno = m.random(spyArraySize);
			char spyPlace = SPY_UNDEFINED;
			short	spyPlacePara = 0;
			int killCount = 0;
			for( int i = spyArraySize; i > 0; --i )
			{
				if( ++spyRecno > spyArraySize )
					spyRecno = 1;

				if( spy_array.is_deleted(spyRecno) )
					continue;

				Spy *spyPtr  = spy_array[spyRecno];

				// town or firm only

				if( (spyPtr->spy_place == SPY_TOWN || spyPtr->spy_place == SPY_FIRM)
					&& spyPtr->cloaked_nation_recno == nation_recno && spyPtr->true_nation_recno != nation_recno )
				{
					spyPlace = spyPtr->spy_place;
					spyPlacePara = spyPtr->spy_place_para;
					break;
				}
			}

			if( spyPlace != SPY_UNDEFINED )
			{
				for( spyRecno = 1; spyRecno <= spyArraySize; ++spyRecno )
				{
					if( spy_array.is_deleted(spyRecno) )
						continue;

					Spy *spyPtr  = spy_array[spyRecno];

					if( spyPtr->spy_place == spyPlace && spyPtr->spy_place_para == spyPlacePara
						&& spyPtr->cloaked_nation_recno == nation_recno &&  spyPtr->true_nation_recno != nation_recno )
					{
						if( m.random(10) < 7 )		// 70% catch
						{
							spyPtr->set_exposed(COMMAND_AUTO);
							++killCount;

							spyPtr->get_loc( viewX, viewY );
						}
					}
				}

				if( killCount > 0 )
				{
					resultStr = text_firm.process( text_firm.get_format_str( 26, "BAWE" ),
						killCount );
				}
			}
		}
		break;

	case WISH_KILL_SPY_NATION:
	// <count> enemy spies in your Kingdom has been exposed and executed!
		{
			int spyArraySize = spy_array.size();
			int spyRecno;
			int killCount = 0;
			for( spyRecno = 1; spyRecno <= spyArraySize; ++spyRecno )
			{
				if( spy_array.is_deleted(spyRecno) )
					continue;

				Spy *spyPtr  = spy_array[spyRecno];

				if( spyPtr->cloaked_nation_recno == nation_recno &&  spyPtr->true_nation_recno != nation_recno )
				{
					if( m.random(10) < 3 )		// 30% catch
					{
						spyPtr->set_exposed(COMMAND_AUTO);
						killCount++;
					}
				}
			}

			if( killCount > 0 )
			{
				resultStr = text_firm.process( text_firm.get_format_str( 27, "BAWF" ),
					killCount );
			}
		}
		break;

	case WISH_CLEAR_SPY_NATION:
	// Every single enemy spy in your Kingdom has been exposed and executed!
		{
			int spyArraySize = spy_array.size();
			int spyRecno;
			for( spyRecno = 1; spyRecno <= spyArraySize; ++spyRecno )
			{
				if( spy_array.is_deleted(spyRecno) )
					continue;

				Spy *spyPtr  = spy_array[spyRecno];

				if( spyPtr->cloaked_nation_recno == nation_recno &&  spyPtr->true_nation_recno != nation_recno )
				{
					if( m.random(10) < 9 )		// 90% catch
					{
						spyPtr->set_exposed(COMMAND_AUTO);
					}
				}
			}

			resultStr = text_firm.get_format_str( 28, "BAWG" );
		}
		break;

	case WISH_HEAL_BUILDING:
	// All of the wounded in a(n) (structure) of yours have been healed of their wounds!
		{
			// select building

			int bestFirm = 0;
			int bestWeight = 0;
			int firmRecno = m.random(firm_array.size());
			for(int i = firm_array.size(); i > 0; --i )
			{
				if( ++firmRecno > firm_array.size() )
					firmRecno = 1;

				if( firm_array.is_deleted(firmRecno) )
					continue;

				Firm *firmPtr = firm_array[firmRecno];

				if( firmPtr->nation_recno != nation_recno	|| firmPtr->under_construction )
					continue;

				// calc. weight

				if( firmPtr->cast_to_FirmCamp() )
				{
					FirmCamp *camp = firmPtr->cast_to_FirmCamp();

					// #### begin Gilbert 1/6 #######//
					// skip empty camp
					if( !camp->overseer_recno && camp->soldier_count == 0 )
						continue;
					// #### end Gilbert 1/6 #######//

					int weight = 0;
					if( camp->overseer_recno )
					{
						weight += unit_array[camp->overseer_recno]->max_hit_points()
							- (int)unit_array[camp->overseer_recno]->hit_points;
					}
					for( int s = 0; s < camp->soldier_count; ++s )
					{
						weight += camp->soldier_array[s].max_hit_points()
							- (int)camp->soldier_array[s].hit_points;
					}

					if( weight > bestWeight )
					{
						bestWeight = weight;
						bestFirm = firmRecno;
					}
				}
				// else if other firms...
			}

			if( bestFirm && bestWeight > 16 )		// don't heal if they are healthy
			{
				Firm *firmPtr = firm_array[bestFirm];

				if( firmPtr->cast_to_FirmCamp() )
				{
					FirmCamp *camp = firmPtr->cast_to_FirmCamp();

					if( camp->overseer_recno )
						unit_array[camp->overseer_recno]->change_hit_points( 200.0f );
					for( int s = 0; s < camp->soldier_count; ++s )
							camp->soldier_array[s].change_hit_points( 200 );
				}
				// else if other firms...

				resultStr = text_firm.process( text_firm.get_format_str( 29, "BAWH" ),
					firm_res.get_build(firmPtr->firm_build_id)->name );

				viewX = firmPtr->center_x;
				viewY = firmPtr->center_y;
			}
		}
		break;

	case WISH_HEAL_NATION:
	// Every soldier in your Kingdom has been healed of his wounds!
		{
			// all units

			for(int unitRecno = unit_array.size(); unitRecno > 0; --unitRecno )
			{
				if( unit_array.is_deleted(unitRecno) )
					continue;

				Unit *unitPtr = unit_array[unitRecno];
				if( unitPtr->nation_recno == nation_recno && unitPtr->hit_points > 0.0f )
				{
					unitPtr->change_hit_points(100.0f);
				}
			}

			// all soldiers

			for( int firmRecno = firm_array.size(); firmRecno > 0; --firmRecno )
			{
				if( firm_array.is_deleted(firmRecno) )
					continue;

				Firm *firmPtr = firm_array[firmRecno];

				if( firmPtr->nation_recno != nation_recno	)
					continue;

				if( firmPtr->cast_to_FirmCamp() )
				{
					FirmCamp *camp = firmPtr->cast_to_FirmCamp();

					for( int s = 0; s < camp->soldier_count; ++s )
						camp->soldier_array[s].change_hit_points( 100 );
				}
				// else if other firms...
			}

			resultStr = text_firm.get_format_str( 30, "BAWI" );
		}
		break;

	case WISH_LOYALTY_BUILDING_S:
	case WISH_LOYALTY_BUILDING_L:
	// The loyalty of the people of (town) has increased by (points) points!
	// The loyalty of everyone in a(n) (structure) of yours has increased by (points) points!
	// The loyalty of the people of (town) has increased to its maximum!
	// The loyalty of everyone in a(n) (structure) of yours has increased to its maximum!
		{
			int loyalInc = (effectId == WISH_LOYALTY_BUILDING_S ? m.random(30)+20 : 100);

			if( m.random(2) == 0 )
			{
				// town
				int townArraySize = town_array.size();
				int townRecno = m.random(townArraySize);
				for( int i = townArraySize; i > 0; --i )
				{
					if( ++townRecno > townArraySize )
						townRecno = 1;

					if( town_array.is_deleted(townRecno) )
						continue;

					Town *townPtr = town_array[townRecno];

					if( townPtr->nation_recno != nation_recno	)
						continue;

					if( townPtr->under_construction )
						continue;

					townPtr->change_loyalty( (float)loyalInc );

					if( loyalInc < 100 )
					{
						resultStr = text_firm.process( text_firm.get_format_str( 31, "BAWJ" ),
							townPtr->town_name(), loyalInc );
					}
					else
					{
						resultStr = text_firm.process( text_firm.get_format_str( 32, "BAWK" ),
							townPtr->town_name() );
					}

					viewX = townPtr->center_x;
					viewY = townPtr->center_y;
					break;
				}
			}
			else
			{
				// firm
				int firmArraySize = firm_array.size();
				int firmRecno = m.random(firmArraySize);
				for( int i = firmArraySize; i > 0; --i )
				{
					if( ++firmRecno > firmArraySize )
						firmRecno = 1;

					if( firm_array.is_deleted(firmRecno) )
						continue;

					Firm *firmPtr = firm_array[firmRecno];

					if( firmPtr->nation_recno != nation_recno	)
						continue;

					if( firmPtr->under_construction )
						continue;

					if( firmPtr->cast_to_FirmCamp() )
					{
						FirmCamp *camp = firmPtr->cast_to_FirmCamp();
						if( camp->overseer_recno )
							unit_array[camp->overseer_recno]->change_loyalty( loyalInc );
						for( int s = 0; s < camp->soldier_count; ++s )
							camp->soldier_array[s].change_loyalty( loyalInc );

						if( loyalInc < 100 )
						{
							resultStr = text_firm.process( text_firm.get_format_str( 33, "BAWL" ),
								firm_res.get_build( firmPtr->firm_build_id )->name, loyalInc );
						}
						else
						{
							resultStr = text_firm.process( text_firm.get_format_str( 34, "BAWM" ),
								firm_res.get_build( firmPtr->firm_build_id )->name );
						}

						viewX = firmPtr->center_x;
						viewY = firmPtr->center_y;
						break;
					}
					else if( firmPtr->cast_to_FirmTrain() )
					{
						FirmTrain *train = firmPtr->cast_to_FirmTrain();
						for( int s = 0; s < train->trainee_count; ++s )
							train->trainee_array[s].change_loyalty( loyalInc );

						if( loyalInc < 100 )
						{
							resultStr = text_firm.process( text_firm.get_format_str( 33, "BAWL" ),
								firm_res.get_build( firmPtr->firm_build_id )->name, loyalInc );
						}
						else
						{
							resultStr = text_firm.process( text_firm.get_format_str( 34, "BAWM" ),
								firm_res.get_build( firmPtr->firm_build_id )->name );
						}

						viewX = firmPtr->center_x;
						viewY = firmPtr->center_y;
						break;
					}
				}
			}
		}
		break;

	case WISH_LOYALTY_NATION_S:
	case WISH_LOYALTY_NATION_L:
	// The loyalty of every subject in your Kingdom has increased by (points) points!
	// The loyalty of every subject in your Kingdom has increased to its maximum!
		{
			int loyalInc = (effectId == WISH_LOYALTY_NATION_S ? m.random(30)+20 : 100);

			// unit_array

			for(int unitRecno = unit_array.size(); unitRecno > 0; --unitRecno )
			{
				if( unit_array.is_deleted(unitRecno) )
					continue;

				Unit *unitPtr = unit_array[unitRecno];
				if( unitPtr->nation_recno == nation_recno )
				{
					unitPtr->change_loyalty( loyalInc );
				}
			}

			// town_array

			for( int townRecno = town_array.size(); townRecno > 0; --townRecno )
			{
				if( town_array.is_deleted(townRecno) )
					continue;

				Town *townPtr = town_array[townRecno];

				if( townPtr->nation_recno != nation_recno	)
					continue;

				if( townPtr->under_construction )
					continue;

				townPtr->change_loyalty( (float) loyalInc );
			}

			// firm_array

			for( int firmRecno = firm_array.size(); firmRecno > 0; --firmRecno )
			{
				if( firm_array.is_deleted(firmRecno) )
					continue;

				Firm *firmPtr = firm_array[firmRecno];

				if( firmPtr->nation_recno != nation_recno	)
					continue;

				if( firmPtr->under_construction )
					continue;

				if( firmPtr->cast_to_FirmCamp() )
				{
					FirmCamp *camp = firmPtr->cast_to_FirmCamp();
					for( int s = 0; s < camp->soldier_count; ++s )
						camp->soldier_array[s].change_loyalty( loyalInc );
				}
				else if( firmPtr->cast_to_FirmTrain() )
				{
					FirmTrain *train = firmPtr->cast_to_FirmTrain();
					for( int s = 0; s < train->trainee_count; ++s )
						train->trainee_array[s].change_loyalty( loyalInc );
				}
			}

			if( loyalInc < 100 )
			{
				resultStr = text_firm.process( text_firm.get_format_str( 35, "BAWN" ),
					loyalInc );
			}
			else
			{
				resultStr = text_firm.get_format_str( 36, "BAWO" );
			}
		}
		break;

	case WISH_COMBAT_BUILDING:
	// The combat level of all soldiers in a(n) (structure) of yours has increased by (points) points!
		{
			float incCombat = m.random(40)+30.0f;
			// select building

			int firmRecno = m.random(firm_array.size());
			for(int i = firm_array.size(); i > 0; --i )
			{
				if( ++firmRecno > firm_array.size() )
					firmRecno = 1;

				if( firm_array.is_deleted(firmRecno) )
					continue;

				Firm *firmPtr = firm_array[firmRecno];

				if( firmPtr->nation_recno != nation_recno	)
					continue;

				if( firmPtr->under_construction )
					continue;

				if( firmPtr->cast_to_FirmCamp() )
				{
					FirmCamp *camp = firmPtr->cast_to_FirmCamp();

					// #### begin Gilbert 1/6 #######//
					// skip empty camp
					if( !camp->overseer_recno && camp->soldier_count == 0 )
						continue;
					// #### end Gilbert 1/6 #######//

					if( camp->overseer_recno )
					{
						unit_array[camp->overseer_recno]->skill.inc_combat_level(incCombat);
						unit_array[camp->overseer_recno]->set_combat_level(-1);
					}

					for( int s = 0; s < camp->soldier_count; ++s )
					{
						if( !camp->soldier_array[s].is_under_training() )
							camp->soldier_array[s].skill.inc_combat_level(incCombat);
					}

					resultStr = text_firm.process( text_firm.get_format_str( 37, "BAWP" ),
						firm_res.get_build(firmPtr->firm_build_id)->name, (int) incCombat );

					viewX = firmPtr->center_x;
					viewY = firmPtr->center_y;
					break;
				}
				else if( firmPtr->cast_to_FirmTrain() )
				{
					FirmTrain *train = firmPtr->cast_to_FirmTrain();

					// #### begin Gilbert 1/6 #######//
					// skip empty training center
					if( train->trainee_count == 0 )
						continue;
					// #### end Gilbert 1/6 #######//

					for( int s = 0; s < train->trainee_count; ++s )
					{
						if( !train->trainee_array[s].is_under_training
							&& train->trainee_array[s].train_combat_potential > 0 )	// avoid increase civilian combat
							train->trainee_array[s].skill.inc_combat_level(incCombat);
					}

					resultStr = text_firm.process( text_firm.get_format_str( 37, "BAWP" ),
						firm_res.get_build(firmPtr->firm_build_id)->name, (int) incCombat );

					viewX = firmPtr->center_x;
					viewY = firmPtr->center_y;
					break;
				}
				// else if other firms...
			}
		}
		break;

	default:
		err_here();
	}

	if( !resultStr[0] )
	{
		// No Effect!
		resultStr = text_firm.get_format_str( 44, "BAW_" );	
		pray_points = pray_points / 2;
	}
	else
	{
		pray_points = 0.0f;	// consume all pray points
	}
	invoked_effect_id = effectId;

	if( firm_recno == firm_array.selected_recno )
	{
		err_when( sizeof(invoked_effect_text) == sizeof(char *) );	// error when invoked_effect_text changed to char * from char[]
		strncpy( invoked_effect_text, resultStr, sizeof(invoked_effect_text) );
		invoked_effect_text[sizeof(invoked_effect_text)-1] = '\0';

		if( viewX >= 0 && viewY >= 0 && is_own() )
		{
			world.go_loc( viewX, viewY );
		}
	}
}
