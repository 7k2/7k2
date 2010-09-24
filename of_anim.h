// Filename    : OF_ANIM.H
// Description : header file of Animal Habitat


#ifndef __OF_ANIM_H
#define __OF_ANIM_H

#include <OFIRM.H>

// ------- define constant -------//

#define MAX_ANIMAL_UNIT   5
#define ANIMAL_TYPE_COUNT 3

// ------- define struct AnimalUnit --------/

struct AnimalUnit
{
	short unit_id;

	short hire_cost;
	short live_point_cost;
	short stay_count;		// how long this unit is going to stay until it leaves the animal habitat if you do not hire him.

	Skill skill;
	int  	combat_level() 	{ return skill.actual_combat_level(&item); }
	int  	skill_level()		{ return skill.actual_skill_level(&item); }
	int  	max_hit_points()	{ return skill.actual_max_hit_points(&item); }

	Item  item;

	short	display_slot_id;

public:
	void	set_hire_cost();
};


// ------- define class FirmAnimal --------/

class FirmAnimal : public Firm
{
public:

	enum { MAX_DISPLAY_SLOT = 5 };

	AnimalUnit		animal_unit_array[MAX_ANIMAL_UNIT];
	short				animal_unit_count;

	static short	selected_unit_id;
	static short	animal_unit_id[ANIMAL_TYPE_COUNT];

public:
	FirmAnimal();
	~FirmAnimal();

	void 		init_derived();
	void		deinit_derived();
	void		free_all_people();

	// ------- interface functions -------//

	void 		put_info(int);
	void 		detect_info();

	// ------ process function -------//

	void		next_day();
	int		kill_unit(int recNo);

	int		can_hire(int recNo);
	int		hire(short recNo);

	//------- AI functions -------//

	void		process_ai();

	//--------- firm drawing functions ----------//

	void		draw(int displayLayer=1);

	//------- multiplayer checking codes -------//

	virtual	UCHAR crc8();
	virtual	void	clear_ptr();

	// --------- type casting function --------//

	virtual FirmAnimal*  cast_to_FirmAnimal() { return this; }

	//---------- save game function ---------//

	int  read_derived_file(File*);

private:
	int		transfer_animal_unit(int recNo);
	void 		add_animal_unit(int unitId, int animalLevel);
	void 		del_animal_unit(int recNo);
	void 		update_add_hire_list();
	void 		update_del_hire_list();

	// ------ protected interface function -------//

	void 		disp_unit_list(int dispY1, int refreshFlag);
	void 		detect_unit_list();
	void		disp_unit_info(int dispY1, int refreshFlag);

	//-------- AI actions ---------//

	void		think_hire_new();
	int		think_del();
};

#endif