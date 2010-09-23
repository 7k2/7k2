//Filename    : OSKILL.H
//Description : Skill class

#ifndef __OSKILL_H
#define __OSKILL_H

#ifndef __OITEM_H
#include <OITEM.H>
#endif 

//--- Define the default skill of the citizens ---//

enum { CITIZEN_COMBAT_LEVEL = 10,
		 CITIZEN_SKILL_LEVEL  = 10,
};

enum { BASIC_COMBAT_TRAIN = 20,		// combat level of a unit that has completed basic training
		 MAX_COMBAT_TRAIN  = 100,		// max combat level through training
		 MAX_COMBAT_BATTLE = 200,		//	max combat level through battling
												// the absolute maximum combat level varies from unit to unit and is stored in Skill::max_combat_level

		 BASIC_SKILL_TRAIN =  20,		// skill level of a unit that has completed basic training
		 MAX_SKILL_TRAIN 	 = 100,		// max skill level that can be achieved through training
};

//-------- Define struct Skill ----------//

class Skill		// currently leadership is only one skill type in the game
{
public:
	unsigned char  skill_potential;			// skill potential
	short 		  	max_combat_level;
	short				max_skill_level;

private:
	short				std_hit_points;			// the hit points when combat level is 100
	float 		  	combat_level;    			// they must be accessed via actual_??()
	float 			skill_level;

public:
	Skill();

	void 				init(int unitId, int initCombatLevel=0, int initSkillLevel=0);

	int  				actual_combat_level(Item* item=0);
	int  				actual_skill_level(Item* item=0);
	int				actual_max_hit_points(Item* item=0);

	void				inc_combat_level(float changeLevel);
	void				inc_skill_level(float  changeLevel);

	void				set_combat_level(int newLevel);
	void				set_skill_level(int newLevel);

	void				set_max_combat_level(int newLevel);
	void				set_max_skill_level(int newLevel);

	void				set_unit_id( int unitId );
};

//---------------------------------------//

#endif
