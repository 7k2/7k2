//Filename    : OBULLET.H
//Description : Header file of Object Bullet
//Owner		  : Alex

#ifndef __OBULLET_H
#define __OBULLET_H

#ifndef __OSPRITE_H
#include <OSPRITE.H>
#endif

#ifndef __OUNIT_H
#include <OUNIT.H>
#endif

#ifndef __OFIRM_H
#include <OFIRM.H>
#endif

#include <OATTACK.H>

//------ define the parent type ------//
enum	{	BULLET_BY_UNIT = 1,
			BULLET_BY_FIRM,
			BULLET_BY_TOWN,
		};

//----------- Define class Bullet -----------//

class Unit;

class Bullet : public Sprite
{
public:
	short	parent_recno;
	short parent_base_obj_recno;

	//char	mobile_type;			// mobile type of the bullet
	char	target_mobile_type;
	float attack_damage;
	short damage_radius;
	// short nation_recno;
	char	fire_radius;

	short	origin_x, origin_y;
	short target_x_loc, target_y_loc;
	short	cur_step, total_step;
	short	z_init;			// starting z
	short	z_dest;			// finish z

	AttackAttribute attack_attribute;

public:
	Bullet();

	virtual void 	init(char parentType, short parentRecno, short targetXLoc, short targetYLoc, char targetMobileType);
	short	get_z();
	void 	process_move();
	int	process_die();
	void 	hit_target(short x, short y);
	void 	hit_building(short x, short y);
	void 	hit_wall(short x, short y);
	float	attenuated_damage(BaseObj *);
	int	check_hit();
	int	warn_target();
	virtual char display_layer();
	virtual int reflect( int baseObjRecno );

	int 	write_file(File* filePtr);
	int	read_file(File* filePtr);

	virtual int	write_derived_file(File* filePtr);
	virtual int	read_derived_file(File* filePtr);

	//-------------- multiplayer checking codes ---------------//
	virtual	UCHAR crc8();
	virtual	void	clear_ptr();
};

//------- Define class BulletArray ---------//

class BulletArray : public SpriteArray
{
public:
	BulletArray(int initArraySize);

	int	create_bullet(short spriteId, Bullet** =NULL);

	short add_bullet(Firm* parentFirm, Unit* targetUnit);		// firm attacks unit
	short add_bullet(Place* parentPlace, Place* targetPlace);		// firm attacks firm
	short add_bullet(Unit* parentUnit, BaseObj* targetObj); //unit attacks everything
	short add_bullet(Town* parentTown, Unit* targetUnit);

	int	add_bullet_possible(short startXLoc, short startYLoc, char attackerMobileType,
									  short targetXLoc, short targetYLoc, char targetMobileType,
									  short targetWidth, short targetHeight, short& resultXLoc, short& resultYLoc,
									  char bulletSpeed, short bulletSpriteId, Unit* emitter = NULL);
	int	bullet_path_possible(short startXLoc, short startYLoc, char attackerMobileType,
										short destXLoc, short destYLoc, char targetMobileType,
										char bulletSpeed, short bulletSpriteId, Unit* emitter = NULL);

	int 	write_file(File* filePtr);
	int	read_file(File* filePtr);

	int	bullet_class_size(int spriteId);

	#ifdef DEBUG
		Bullet* operator[](int recNo);
	#else
		Bullet* operator[](int recNo)   { return (Bullet*) get_ptr(recNo); }
	#endif
};

extern BulletArray bullet_array;

//-----------------------------------------//

#endif

