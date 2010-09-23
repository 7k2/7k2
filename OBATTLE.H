//Filename    : OBATTLE.H
//Description : Header file for Battle object

#ifndef __OBATTLE_H
#define __OBATTLE_H

//---------- define constant -----------//

#define MAX_GEN_MAP_STEP 	100

//---------- Define class Battle --------//

struct NewNationPara;
class  Nation;
class  Place;

class Battle
{
private:
	short placement_range_x1, placement_range_y1, placement_range_x2, placement_range_y2;		// the range where new objects should be placed

public:
	Battle();

	void	run(NewNationPara* mpGame=0, int mpPlayerCount=0);
	void	run_loaded();

	//---------------------------------//

	void	set_placement_range(int rangeX1, int rangeY1, int rangeX2, int rangeY2);
	void	reset_placement_range();
	void 	inverse_placement_range();
	void	init_uniqueness();

	void 	create_ai_nation();

	virtual void  create_pregame_object()		{;}
	void  create_pregame_object(NewNationPara *mpGame, int mpPlayerCount);

	int 	create_one_human_asset(Nation* nationPtr);
	int 	create_one_monster_asset(Nation* nationPtr);
	int 	create_human_town_unit(Nation* nationPtr);
	int 	create_human_town(Nation* nationPtr);
	void  create_independent_town(int addCount);
	void  create_road();
	int 	create_unit(int nationRecno, int unitId, int rankId, int nextXLoc1= -1, int nextYLoc1= -1, int nextXLoc2= -1, int nextYLoc2= -1);
	int 	create_town(int nationRecno, int raceId, int xLoc, int yLoc, int initPop=0);
	int 	create_town_next_to_place(Place* placePtr, int nationRecno, int raceId, int initPop=0);
	int 	create_town_random_loc(int nationRecno, int raceId, int& xLoc, int& yLoc, int initPop=0);
	int 	create_firm(int xLoc, int yLoc, int nationRecno, int firmId, int firmRaceId);
	int 	create_firm_next_to_place(Place* placePtr, int firmId, int firmRaceId, int nationRecno= -1, short distance= 0);
	void  create_monster_lair(int nationRecno, int addCount);
	void 	create_random_item();

	int   is_space(int xLoc1, int yLoc1, int xLoc2, int yLoc2, char mobileType);
	void  center_disp_on_player();
};

extern Battle battle;

//---------------------------------------//

#endif