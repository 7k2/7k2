//Filename    : OATTACK.H
//Description : Header file of Attack Attribute

#ifndef __OATTACK_H
#define __OATTACK_H

//-------- Define struct DefenseAttribute -------//

struct DefenseAttribute
{
	char	 sturdiness;		
	char	 is_wood;

	void	init(char s, char w) { sturdiness = s;
											is_wood = w;	}
};

//-------- Define struct AttackAttribute -------//

struct AttackAttribute
{
	char	 sturdiness;		
	char	 explosiveness;		
	char	 heat;
	char	 wood_favour;

	void	init(char s, char e, char h, char w) { sturdiness = s;
																explosiveness = e;
																heat = h;
																wood_favour = w; }

	float	 effectiveness ( DefenseAttribute & );
};

#endif
