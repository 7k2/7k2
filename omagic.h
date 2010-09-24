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

// Filename    : OMAGIC.H
// Description : Header file for Magic class
// Ownership   : Ban

#ifndef __MAGIC_H
#define __MAGIC_H
 
//--------- Define class Magic ----------//
struct Particle
{
//	Particle *prev, *next;
	int pos_x;	int prev_x;	int dir_x;
	int pos_y;	int prev_y;	int dir_y;
	int life;
	short color;
};
struct Lighting_Particle
{
//	Lighting_Particle *prev, *next;
	int pos_x;	int prev_x; int dir_x;
	int pos_y;	int prev_y; int dir_y;
//	int thickness;
//	int lightness;
	int random;
};

class VgaBuf;
class Magic
{
public:
	unsigned short *check_table;
	unsigned short *color_table;
	unsigned short *dist_table;
	unsigned seed;				// last random number
	char effect_testing_flag;
	Particle *h_particle;
//	Lighting_Particle *h_l_particle;

public:
	Magic();
	~Magic();
	void		test_flag();
	void		draw_light_angle(VgaBuf *vgabuf, int x1, int y1, int radius,
								 int thickness, int lightness, short R1, short G1, short B1, 
								 short R2, short G2, short B2, int mode); 
	void		generate_particle(int x1, int y1, int x2, int y2, int dirx, int diry, int type, int var, int mode,
									int number, int birth_time, int lifecount, int lifeLimit, int lifeOffset = 0);
	void		draw_particle(int x1, int y1, int x2, int y2, int dirx, int diry, int type, int var, int size,
								VgaBuf *vgabuf, int mode, int number, int birth_time, int lifecount, int life_limit, int lifeOffset = 0);
	short		add_blend(short *point, short R, short G, short B, int mode);

	short		add_blend2(short *point, short *point2, int mode);

	short		add_blend3(short *point, short R, short G, short B, int mode, int par);

	short		mins_blend(short *point, short R, short G, short B, int mode);

	short		shading(short *origin, short *point, short ratio, int mode);

	short		draw(short R, short G, short B, int mode);
	
	double	dist(double dx, double dy);
	
	void		init(int x1, int y1, int x2, int y2);

	void		straight_light_beam(VgaBuf *vgabuf, int x1, int y1, int x2, int y2,
									int thickness, int lightness, short R1, short G1, short B1, 
									short R2, short G2, short B2, int mode, char angle);
	void		straight_light_beam2(VgaBuf *vgabuf, int x1, int y1, int x2, int y2,
									int thickness, int lightness, short R1, short G1, short B1, 
									short R2, short G2, short B2, int mode, char angle);
	void		draw_light_beam(VgaBuf *vgabuf, int x1, int y1, int x2, int y2,
								int thickness, int lightness, short R1, short G1, short B1, 
								short R2, short G2, short B2, int mode, char angle, char fast = 1);
	void		draw_light_ball(VgaBuf *vgabuf, int x1, int y1, int radius,
								int thickness, int lightness, short R1, short G1, short B1, 
								short R2, short G2, short B2, int mode);
	void		zigzag_light_beam(VgaBuf *vgabuf, double x1, double y1, double x2, double y2,
								  int thickness, int lightness, short R1, short G1, short B1, 
								  short R2, short G2, short B2, int mode, unsigned rough, int span);
	void		draw_circle(VgaBuf *vgabuf, int x1, int y1, int radius, char r_x, char r_y,
							  short R1, short G1, short B1, int mode, int dir); 
	
	void		draw_circle2(VgaBuf *vgabuf, int x1, int y1, int radius, char r_x, char r_y,
								short R1, short G1, short B1, int mode, int dir);
	
	void		generate_lighting_particle(int x1, int y1, int x2, int y2, int number, int lifecount, 
											Lighting_Particle *h_l_particle); 
	void		draw_lighting(int x1, int y1, int x2, int y2, int thickness, int lightness, short R1, short G1, short B1, 
						  short R2, short G2, short B2, VgaBuf *vgabuf, int var, int mode, int number, int lifecount, int level);
	void		draw_magic_one(VgaBuf *vgabuf, int x1, int y1, int mode, int count);
	void		draw_magic_two(VgaBuf *vgabuf, int x1, int y1, int x2, int y2, int diff, int mode, int curStep, int totalStep); 
	void		draw_magic_three(VgaBuf *vgabuf, int x1, int y1, int x2, int y2, int mode, int count);
	void		draw_magic_four(VgaBuf *vgabuf, int x1, int y1, int x2, int y2, int mode, int count);
	void		draw_magic_five(VgaBuf *vgabuf, int x1, int y1, int x2, int y2, int mode, int count); 
	void		draw_magic_six(VgaBuf *vgabuf, int x1, int y1, int x2, int y2, int mode, int count); 
	void		draw_magic_seven(VgaBuf *vgabuf, int x1, int y1, int x2, int y2, int mode, int count); 
	void		draw_magic_eight(VgaBuf *vgabuf, int x1, int y1, int x2, int y2, int mode, int count); 
	void		draw_magic_nine(VgaBuf *vgabuf, int x1, int y1, int x2, int y2, int mode, int curStep, int totalStep);
	void		draw_magic_nine_die(VgaBuf *vgabuf, int x1, int y1, int mode, int count); 
	void		draw_magic_ten(VgaBuf *vgabuf, int x1, int y1, int x2, int y2, int mode, int curStep, int totalStep, int delay);
	void		draw_magic_eleven(VgaBuf *vgabuf, int x1, int y1, int mode, int curStep); 
	void		draw_magic_twelve(VgaBuf *vgabuf, int x1, int y1, int mode, int curStep, int nationRecno);
	void		draw_magic_twelve_die(VgaBuf *vgabuf, int x1, int y1, int mode, int curStep, int nationRecno, int random);
	void		draw_magic_thirteen(VgaBuf *vgabuf, int x1, int y1, int mode, int curStep);
	void		draw_magic_forteen(VgaBuf *vgabuf, int x1, int y1, int mode, int curStep, int curDir);
	void		draw_magic_fifteen(VgaBuf *vgabuf, int x1, int y1, int mode, int curStep);
	void		draw_magic_firm_die(VgaBuf *vgabuf, int x1, int y1, int mode, int curStep);
		
protected:
	unsigned rand_seed();			// shuffle and return seed
};
extern Magic magic;

#endif

