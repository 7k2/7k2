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

//Filename    : OTECHRES.H
//Description : class Tech

#ifndef __OTECHRES_H
#define __OTECHRES_H

#ifndef __ORESX_H
#include <oresx.h>
#endif

//------ define struct TechClassRec ---------//

struct TechClassRec
{
	enum { CODE_LEN=8, DESC_LEN=40, RACE_CODE_LEN=8, RACE_ID_LEN=2 };

	char class_code[CODE_LEN];
	char desc[DESC_LEN];

	char race_code[RACE_CODE_LEN];
	char race_id[RACE_ID_LEN];
};

//------ define struct TechRec ---------//

struct TechRec
{
	enum { CODE_LEN=8, DESC_LEN=40, MAX_TECH_LEVEL_LEN=3, COMPLEX_LEVEL_LEN=3, ID_LEN=3,
			 RACE_CODE_LEN=8, RACE_ID_LEN=3 };


	char tech_code[CODE_LEN];
	char class_code[CODE_LEN];
	char desc[DESC_LEN];

	char max_tech_level[MAX_TECH_LEVEL_LEN];
	char complex_level[COMPLEX_LEVEL_LEN];

	char unit_code[CODE_LEN];
	char firm_code[CODE_LEN];
	char parent_tech_code[CODE_LEN];
	char parent_tech_level;
	char parent_tech_code2[CODE_LEN];
	char parent_tech_level2;
	char advantage_race[RACE_CODE_LEN];

	char class_id[ID_LEN];
	char unit_id[ID_LEN];
	char firm_id[ID_LEN];
	char parent_tech_id[ID_LEN];
	char parent_tech_id2[ID_LEN];
	char advantage_race_id[RACE_ID_LEN];
};

//-------- define struct TechClass ----------//

#pragma pack(1)
class TechClass
{
public:
	enum { DESC_LEN=40 };

	short class_id;
	char	desc[DESC_LEN+1];
	short	race_id;

	short first_tech_id;
	short tech_count;

	int   total_tech_count(int nationRecno);			// the number of tech researched by each nation in this tech class
	int 	researchable_tech_count(int nationRecno, int raceId=0);

public:
	void 	random_init(int nationRecno, int minTechCount, int maxTechCount);
};
#pragma pack()

//-------- define struct TechInfo ----------//

#pragma pack(1)
class TechInfo
{
public:
	enum { DESC_LEN = 40 };

	short tech_id;
	short class_id;
	char	desc[DESC_LEN+1];

	short common_max_tech_level;		// maximum level for this technology
	short complex_level;

	short unit_id;
	short firm_id;
	short race_id;					// if the tech is race specific
	short parent_tech_id;
	char  parent_tech_level;
	short parent_tech_id2;
	char	parent_tech_level2;

	int	is_monster_tech()  { return race_id < 0; }
	int	is_human_tech()	 { return race_id >= 0; }

	char* tech_des();			// description of the technology

	int	live_points_cost() 	{ return race_id < 0 ? complex_level * 20 : 0; }

	//-------- dynamic game vars --------//

	short	 available_level;		// this tech is only available for research if available_level >= 1. This is for campaign only.
	char   nation_tech_level_array[MAX_NATION];			// each nation's the technology level of this unit,
	char	 nation_research_progress_array[MAX_NATION];
	char	 advantage_race_id;			// can research extra level if same race

public:
	void   set_nation_tech_level(int nationRecno, int techLevel);
	int    get_nation_tech_level(int nationRecno)						{ return nation_tech_level_array[nationRecno-1]; }
	void	 disable_know(int nationRecno);

	int 	 can_research(int nationRecno, int checkLivePoint=1);
	int 	 is_parent_tech_invented(int nationRecno);
	short	 max_tech_level(int nationRecno);

	int 	 is_useful_to_nation(int nationRecno);
	void 	 enable_parent_tech(int nationRecno);
};
#pragma pack()

//------ define class TechRes ----------//

class TechRes
{
public:
	char        init_flag;

	short       tech_class_count;
	short       tech_count;
	short			total_tech_level_array[1+MAX_RACE];		// the sum of research levels of all technology, for each nation race

	TechClass* 	tech_class_array;
	TechInfo*  	tech_info_array;

public:
	TechRes();

	void        init();
	void        deinit();

	void			second_init();

	void 			init_nation_tech(int nationRecno);
	void 			inc_all_tech_level(int nationRecno);
	void			inc_tech_class_level(int nationRecno, int techClass);

	int			researchable_tech_count(int nationRecno, int raceId, int checkLivePoint=1);
	short			total_tech_level(int nationRecno);		// the sum of research levels of all technology, for each nation race
	int			suggest_next_tech(int nationRecno, int finishedTechId);
	int			find_researchable_requirement(int nationRecno, int techId, int recurCount=0);

	int         write_file(File*);
	int         read_file(File*);

   #ifdef DEBUG
		TechInfo*   operator[](int techId);      // pass techId as recno
		TechClass*  tech_class(int techClassId);
	#else
		TechInfo*   operator[](int techId)			{ return tech_info_array+techId-1; }
		TechClass*  tech_class(int techClassId)   { return tech_class_array+techClassId-1; }
	#endif

private:
	void        load_tech_class();
	void        load_tech_info();
};

extern TechRes tech_res;

//------------------------------------//

#endif
