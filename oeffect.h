// Filename    : OEFFECT.H
// Description : header file of effect array

#ifndef __OEFFECT_H
#define __OEFFECT_H

#include <OSPRITE.H>

class Effect : public Sprite
{
public:
	char	layer;
	int	life;
	char	program_effect_flag;
	char	program_effect_id;

public:
	Effect();
	~Effect();

	void	init(short spriteId, short startX, short startY, char initAction, char initDir, char dispLayer, int effectLife);
	void	pre_process();
	void	process_idle();
	int	process_die();
	void	draw(int, int drawFlag);
	void	program_effect_draw();
	void	update_abs_pos(SpriteFrame *spriteFrame);

	static void create(short spriteId, short startX, short startY, char initAction, char initDir, char dispLayer, int effectLife);
};

// to add an effect to effect_array,
// Effect *effectPtr = new Effect;
// effectPtr->init(...)
// effect_array.add(effectPtr);
// or call Effect::create();

extern SpriteArray effect_array;

#endif