// Filename    : OF_TRAIN.H
// Description : Header file of FirmTrain base class of stable and college

#ifndef __OF_TRAIN_H
#define __OF_TRAIN_H

#include <OFIRM.H>
#include <OTRAINEE.H>

//------ Define constant ---------//

#define MAX_TRAINEE  		8

//----- define firm menu mode --------//

enum
{
	FIRM_TRAIN_MENU_MAIN,
	FIRM_TRAIN_MENU_TRAIN,
	FIRM_TRAIN_MENU_SPY,
};

//----- define constants for Trainee::train_type && FirmTrain::cur_train_type ---------//

enum
{
	TRAIN_UNIT_CIVILIAN = 0,
	TRAIN_UNIT_INFANTRY,
	TRAIN_UNIT_SPECIAL,

	TRAIN_UNIT_TYPE_MAX				// keep it last
};

//------- define class FirmTrain --------//

class FirmTrain : public Firm
{
public:
	Trainee		trainee_array[MAX_TRAINEE];
	char			trainee_count;
	char			selected_trainee_id;

	//-------- member on spy ---------//

	char			player_spy_count;

	//------ AI functions for returning values -----//

	virtual int total_combat_level();

	//------- static member -------//

	static char	firm_menu_mode;
	static int	cur_train_type;
	static int	required_linked_firm[TRAIN_UNIT_TYPE_MAX];

public:
	FirmTrain();

	//-------- type casting -------//

	FirmTrain *		cast_to_FirmTrain()      { return this; }

	//-------- interface functions ----------//

	void				put_info(int refreshFlag);
	void				detect_info();
	int				should_show_info();
	int				explore_for_player();
	virtual int		represent_attribute(int traineeId) { return 0; }

	void			disp_edit_mode(int& refreshFlag, int& dispY1, int dispBG=1);
	int			detect_edit_mode();

	//------- process ---------//

	virtual void	next_day();

	//------- functions on trainee --------//

	virtual void	recruit_trainee( int townRecno, int unitPara, char remoteAction );
	virtual int		mobilize_trainee(int traineeId, char remoteAction );
	virtual void	kill_trainee(int traineeId );
	virtual int 	can_recruit(int townRecno, int trainType);
	virtual void	cancel_train_trainee( int traineeId, char remoteAction );

	void				train_unit();
	void				free_all_people();
	virtual void   change_nation(int newNationRecno);

	int				can_set_active_link(int townRecno);
	void				set_active_link(int townRecno, char remoteAction);

	// ------ information function -------//

	virtual int total_military_combat_level();		// for military report
	virtual int total_military_count();

	//------- multiplayer checking codes -------//

	virtual	UCHAR crc8();
	virtual	void	clear_ptr();

protected:

	virtual int		create_trainee_unit(Trainee& thisTrainee);

			  void	basic_train();
	virtual void	advanced_train();

	void				validate_trainee();

	//-------- protected interface functions ---------//

	virtual void	disp_firm_info(int dispY1, int refreshFlag );
	virtual void	detect_firm_info();
	void				disp_trainee_list(int dispY1, int refreshFlag );
	int				detect_trainee_list();
	void				disp_trainee_info(int dispY1, int refreshFlag );
	void				detect_trainee_info();

	virtual void	disp_train_menu(int refreshFlag);
	virtual void	detect_train_menu();

	virtual void	disp_spy_menu(int refreshFlag);
	virtual void	detect_spy_menu();
};

//--------------------------------------------------//

#endif