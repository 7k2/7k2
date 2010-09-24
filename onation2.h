//Filename    : ONATION2.H
//Description : Header for derived Nation classes

#ifndef __ONATION2_H
#define __ONATION2_H

#ifndef __ONATION_H
#include <ONATION.H>
#endif

//------ Define dervied nation class id.  --------//

enum { NATION_HUMAN = 1,
		 NATION_MONSTER,					// Fryhtan nations in standalone games
		 NATION_EAST_WEST_HUMAN1,		// The human nation in the EastWest campaign
};

//------ Define class NatoinHuman -------//

class NationHuman : public Nation
{
public:
	virtual void	process_ai();
	virtual int 	process_ai_main(int mainActionId);


};

//------ Define class NationMonster -------//

class NationMonster : public Nation
{
public:
	virtual void	process_ai();

protected:
	virtual int 	process_ai_main(int mainActionId);
	virtual void 	think_diplomacy();
			void 	think_attack_hostile_nation();
			void 	think_buy_independent_lair();
			int		think_acquire_live_points();
};

//-----------------------------------------//

#endif