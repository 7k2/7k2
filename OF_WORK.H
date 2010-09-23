//Filename    : OF_WORK.H
//Description : Header file of Object FirmWork

#ifndef __OF_WORK_H
#define __OF_WORK_H

#ifndef __OFIRM_H
#include <OFIRM.H>
#endif

//--------- Define constant ----------//

#define MAX_WORKER         		20       // maximum no. of workers in a firm

#define MAX_CARGO					 	 9			// maximum no. of goods icons shown on the zoom map

#define WORKER_YEAR_SALARY    	 5

//------- Define struct Worker -------//

struct Worker
{
	short	town_recno;
};

//------- Define class FirmWork -------//

class FirmWork : public Firm
{
public:
	//-------- worker vars ----------//

	Worker  	worker_array[MAX_WORKER];
	char    	worker_count;       		// number of workers currently working in the firm
	char	  	needed_worker_count;		// number of workers we needed
	char    	selected_worker_id;

	short		productivity;

	int 		available_workforce();
	int		is_operating()		{ return productivity > 0; }

public:
	FirmWork();

	void		deinit_derived();
	void		free_all_people();

	//------- process function ---------//

	void 		next_day();
	void		pay_expense();

	virtual void process_ai();

	//------- worker function ----------//

	void 		set_needed_worker_count(int neededWorkerCount, char remoteAction);

	void 		recruit_worker();
	void 		resign_all_worker();
	void 		resign_worker(int workerId);
	void		kill_all_worker();

	FirmWork* cast_to_FirmWork() 	{ return this; }

	//-------- interface functions ----------//

	void		put_info(int refreshFlag);
	void		detect_info();

	void		disp_edit_mode(int& refreshFlag, int& dispY1, int dispBG=1);
	int		detect_edit_mode();

	//------- multiplayer checking codes -------//

	virtual	UCHAR crc8();
	virtual	void	clear_ptr();

protected:
	void		calc_productivity();
	void 		process_independent_town_worker();

	//------- protected interface functions -------//

	virtual void disp_firm_info(int dispY1, int refreshFlag);
	virtual void detect_firm_info();
	void		disp_worker_list(int dispY1, int refreshFlag);
	void		detect_worker_list();
	void		disp_worker_info(int dispY1, int refreshFlag);
	void		detect_worker_info();

	//------- AI functions --------//

	virtual int	think_adjust_workforce()	{ return 0; }

	int		think_inc_worker_supply();
	int 		ai_build_neighbor_town(int builderUnitRecno=0);
	int 		most_jobless_race();
	int 		race_jobless_pop(int raceId, int excludedTownRecno);
	int 		most_jobless_town(int raceId, int excludedTownRecno);
	int 		is_worker_already_coming();
};

//--------------------------------------//

#endif