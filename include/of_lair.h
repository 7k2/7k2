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

// Filename    : OF_LAIR.H
// Description : Header file of FirmFort

#ifndef __OF_LAIR_H
#define __OF_LAIR_H

#include <of_camp.h>

// ------ define menu mode ----------//

enum
{
	FIRM_LAIR_MENU_RESEARCH = FIRM_MENU_CAMP_LAST,
	FIRM_LAIR_MENU_GRANT,
	FIRM_LAIR_MENU_LAST					// keep this last for subclass to go on
};
 
//------ Define constant ----------//

#define	RECRUIT_MONSTER_COST							50
#define  MONSTER_COLLECT_TOWN_TRIBUTE_LOYALTY   1	 		// Monster Lairs can collect tribute from the independent towns if their resistance is lower than 30
#define  MONSTER_TOWN_TRIBUTE_PER_UNIT				5			// The amount of money can be collected per unit from independent towns


//------ Define class FirmLair ------//

class FirmLair : public FirmCamp
{
public:
	FirmLair();

	void		init_derived();

	// -------- function on casting --------//

	virtual FirmLair*		cast_to_FirmLair()		{ return this; }

	// -------- overloaded interface functions ----------//

	void 		put_info(int refreshFlag);
	void 		detect_info();
	void		disp_camp_info(int dispY1, int refreshFlag);
	void		detect_camp_info();
	int  		should_show_info();

	// ------- function of processing ------//

	void		next_day();
	void		pay_expense();
	virtual void change_nation(int newNationRecno);

	int 		can_grant_to_non_own_lair(int grantNationRecno, int checkCash);
	int 		grant_to_non_own_lair(int grantNationRecno, int remoteAction);
	int 		grant_to_non_own_lair_cost();

	//--------- AI functions ---------//

	virtual void process_ai();
	virtual	int ai_should_close();
				int has_resisting_linked_town();
	virtual 	int ai_combat_level_needed();
		     void ai_build_lair(Place* placePtr);

	//---------- recuit soldier functions ----------//

	void		recruit_soldier(char remoteAction, char noCost=0);
	void 		cancel_train_soldier(int soldierId, char remoteAction);
	int		can_recruit();
	void		natural_breed();

	// --------- research function ----------//

	void		buy_tech(int techId, char remoteAction );

	//------------ other functions ---------//

	  int 	collect_town_tribute(int collectNow=1);

	//----- derived function from BaseObj -----//

	virtual bool	can_accept_assign(int unitRecno, int actionNationRecno= -1);
	virtual void 	being_attack_hit(BaseObj* attackerObj, float damagePoint);

	//------- multiplayer checking codes -------//

	virtual	UCHAR crc8();
	virtual	void	clear_ptr();

protected:
	virtual void	disp_research_menu(int refreshFlag);
	virtual void	detect_research_menu();

			  void   disp_grant_menu(int refreshFlag);
			  void 	detect_grant_menu();

private:

	//------- AI functions --------//

	void 				think_attack_linked_town();
	int  				think_repress_new_town();
	virtual void 	think_recruit(int combatLevelDiff=0);
	virtual int		ai_recruit(int recruitCount);
			  int 	ai_get_soldier_from_other_lair();
			  int 	think_attack_competing_camp();

			  void 	think_build_special();
			  int 	should_build_special(int firmId);

			  int		think_build_kharsuf_special();
			  int 	think_build_bregma_special();
			  int		think_build_kerassos_special();
			  int		think_build_minotauros_special();
			  int		think_build_grokken_special();
			  int		think_build_ezpinez_special();
			  int 	think_build_exovum_special();

			  int		think_buy_tech();
			  int 	think_tech_to_buy(short* techIdArray, short* techPrefArray, int techCount);
			  int 	have_excess_live_point(int techId, int excessLivePoints);

			  int 	think_kharsuf_buy_tech();
			  int 	think_bregma_buy_tech();
			  int 	think_kerassos_buy_tech();
			  int 	think_ezpinez_buy_tech();
			  int 	think_exovum_buy_tech();
			  int 	think_grokken_buy_tech();
			  int 	think_minotauros_buy_tech();
};

//-----------------------------------------------//

#endif