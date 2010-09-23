//Filename    : OBASEOBJ.H
//Description : Class BaseObj

#ifndef __OBASEOBJ_H
#define __OBASEOBJ_H

#ifndef __ODYNARRB_H
#include <ODYNARRB.H>
#endif

#include <OATTACK.H>
//------- define BaseObj type --------//

enum { OBJ_UNIT=1,
		 OBJ_FIRM,
		 OBJ_TOWN
	  };

//------- define class BaseObj --------//

class Unit;
class Firm;
class Town;
class Place;
class Nation;

class BaseObj
{
public:
	//------- identity vars -------//

	short base_obj_recno;		// its recno in base_obj_array

	char	obj_type;
	short obj_extern_recno;		// the recno of the derived object in their own arrays.

	//------- game vars -----------//

	unsigned char is_ai;	// whether Computer AI control this firm or not
	unsigned char already_killed;	// whether this firm has been killed or not
//	char    	is_ai;              // whether Computer AI control this firm or not
	virtual int	is_own();

	char    	nation_recno;
	virtual int true_nation_recno()	{ return nation_recno; }
	Nation* 	nation_ptr();

	char		race_id;
	int		is_human()		{ return race_id>0; }
	int		is_monster()	{ return race_id<0; }
	int		monster_id()	{ return -race_id; }

	int 	 	last_attack_date;		 // date which the object was last involved in an attack
	short		last_attack_nation_recno;

	DefenseAttribute defense_attribute;
	
	//------- location vars and functions --------//

	virtual int obj_loc_width() 	{ return 0; }
	virtual int obj_loc_height()  { return 0; }

	virtual int	obj_loc_x1()		{ return 0; }
	virtual int	obj_loc_y1()		{ return 0; }
	virtual int	obj_loc_x2()		{ return 0; }
	virtual int	obj_loc_y2()		{ return 0; }

	virtual int	obj_mobile_type()	{ return 0; }
	virtual int obj_is_visible()	{ return obj_loc_x1() >= 0; }

	virtual int obj_region_id()	{ return 0; }

	virtual int is_visible()      { return 1; }
	virtual int	is_stealth();
	virtual int explore_for_player();

	//------- combat vars -------//

	virtual float obj_armor()			{ return 0; }
	virtual int   obj_power()			{ return 0; }
	virtual int	  obj_can_attack(int offensiveAttackOnly)	{ return 0; }

	//-------- hit points vars ---------//

	float 	hit_points;
	virtual 	int max_hit_points()		{ return 0; }

	//-------- distance function --------//

	int		area_distance( BaseObj *);

	//------- multiplayer checking codes -------//

	virtual	UCHAR crc8();
	virtual	void	clear_ptr();

	//---- functions for casting to derived classes ----//

	virtual Unit* cast_to_Unit() 	{ return 0; }
	virtual Firm* 	 cast_to_Firm() 		{ return 0; }
	virtual Town* 	 cast_to_Town() 		{ return 0; }
	virtual Place*  cast_to_Place() 		{ return 0; }

public:
	BaseObj();

	virtual bool	can_accept_assign(int unitRecno, int actionNationRecno= -1)	{ return 0; }

	virtual void 	being_attack_hit(BaseObj* attackerObj, float damagePoint);
	virtual void   being_killed(BaseObj* attackerObj);
			  void 	being_attack_hit2(BaseObj* attackerObj);
	        float	calc_live_points_bonus();

	virtual void	change_nation(int newNationRecno);
};

//------- define class BaseObjArray --------//

class BaseObjArray : public DynArrayB
{
public:
	BaseObjArray();

	void		init();
	void		deinit();

	int   	add(Unit* unitPtr);
	int   	add(Firm*   firmPtr);
	int   	add(Town*   townPtr);

	void		del(int recNo);
	void		debug_validate();			

	#ifdef DEBUG
		BaseObj* operator[](int recNo);
	#else
		BaseObj* operator[](int recNo)   { return (BaseObj*) get_ptr(recNo); }
	#endif

	int   	is_deleted(int recNo);
	
	int		write_file(File *);
	int		read_file(File *);
};

extern BaseObjArray base_obj_array;

//-------------------------------------//

#endif

