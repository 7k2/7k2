// Filename    : OTIPS.H
// Description : header file of tips resource


#ifndef __OTIPSRES_H
#define __OTIPSRES_H


// tips classes
enum
{
	TIPS_CLASS_HUMAN = 1,
	TIPS_CLASS_FRYHTAN,
	TIPS_CLASS_CAMPAIGN,
};


// ------- define struct TipsInfo -----------//

struct TipsInfo
{
	short	tips_class;
	short	line_count;
	int	text_start_offset;
};


// ------- define struct TipsClass -----------//

struct TipsClass
{
	short	tips_class;
	short	first_tips;
	short	tips_count;
	char	is_active;
	char	dummy;

	void	set_active();
};


// ------- define class TipsRes -----------//

class TipsRes
{
public:
	int			init_flag;
	char*			tips_text;

	TipsInfo*	tips_info_array;
	int			tips_info_count;

	TipsClass*	tips_class_array;
	int			tips_class_count;

	int			current_tips;
	int			box_active;

public:
	TipsRes();
	~TipsRes();

	void	init();
	void	deinit();

	// -------- tips class function ------//

	TipsClass *get_class(int);
	void	deactive_all_classes();

	// -------- tips info function -------//

	TipsInfo *operator[](int);
	char *get_text(int tipsId);
	void	random_select();
	void	next();		// choose next tips
	void	prev();		// choose prev tips

	// ------- display function ---------//

	void	disp(int x, int y );
	int	detect();

private:
	void load_info();
};


extern TipsRes tips_res;

#endif
