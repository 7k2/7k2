//Filename   : OU_GOD.CPP
//Description: Unit God header file

#ifndef __OU_GOD_H
#define __OU_GOD_H

#ifndef __OUNIT_H
#include <OUNIT.H>
#endif

#ifndef __OFIRM_H
#include <OFIRM.H>
#endif

//----------- Define constant ------------//

enum { DO_CAST_POWER_RANGE = 3 };

//----------- Define class God -----------//

class UnitGod : public Unit
{
public:
	short god_id;
	short base_firm_recno;		// recno of the seat of power which creates and supports this god unit
	short	cast_origin_x, cast_origin_y;
	short cast_target_x, cast_target_y;

	// -------- ai variable -------//

	int	ai_para;			// custom variable used by ai

public:
	UnitGod();
				void pre_process();
				int  process_attack();

				void disp_info(int refreshFlag);
				void detect_info();
				void disp_hit_point();
				int  send_hotkey(unsigned scanCode, unsigned skeyState);	// called by sys::process_key and functions it calls

				int  right_click( int xLoc, int yLoc, int testing );

				void cast_power(int castXLoc, int castYLoc);
	virtual int can_attack_target(int targetObjRecno);

	//-------------- multiplayer checking codes ---------------//

	virtual	UCHAR crc8();
	virtual	void	clear_ptr();

	//--------- casting function ----------//

	UnitGod*	cast_to_UnitGod()	{ return this; }

protected:
//	virtual int can_attack_target(int targetObjRecno);

	virtual void attack_consume_power();
	int	consume_power_pray_points();

	void cast_on_loc(int castXLoc, int castYLoc);
	void cast_on_unit(int unitRecno, int divider);
//	void cast_on_worker(Worker* workerPtr, int nationRecno, int divider);

//	void viking_summon_rain();
//	void viking_summon_tornado();
	void indian_summon_tornado();
	void carth_cast_quake();
	void japanese_kill_spy();
	void japanese_cast_inn(int firmRecno);

	void roman_cast_power(int unitRecno, int divider);
	void greek_cast_power(int unitRecno, int divider);

//	void japanese_cast_power(int unitRecno, int divider);
	void mongol_cast_power(int unitRecno, int divider);

	void egyptian_cast_power(int unitRecno, int divider);
	void persian_cast_power(int unitRecno, int divider);
	void indian_cast_power(int unitRecno, int divider);
/*
	void persian_cast_power(Worker* workerPtr, int nationRecno, int divider);
	void japanese_cast_power(Worker* workerPtr, int nationRecno, int divider);
	void maya_cast_power(Worker* workerPtr, int nationRecno, int divider);
#ifdef AMPLUS
	void egyptian_cast_power(Worker *workerPtr, int nationRecno, int divider);
	void indian_cast_power(Worker *workerPtr, int nationRecno, int divider);
	void zulu_cast_power(Worker *workerPtr, int nationRecno, int divider);
#endif
*/
	//--------- AI functions ----------//

	void process_ai();
	void think_norman_god();	// attack
	void think_viking_god();	// attack
	void think_celt_god();		// attack
	void think_roman_god();		// as same as zulu god
	void think_greek_god();		// protection
	void think_carth_god();		// earthquake
	void think_chinese_god();	// attack
	void think_japanese_god();	// clear spy
	void think_mongol_god();	// betray to your nation
	void think_egyptian_god();	// same as old egyptian god
	void think_persian_god();	// reveal building info
	void think_indian_god();	// add tornado

	int  think_god_attack_target(int& targetXLoc, int& targetYLoc);
	/*
	void think_dragon();
	void think_maya_god();
	void think_phoenix();
	void think_viking_god();
	void think_persian_god();
	void think_chinese_dragon();
	void think_japanese_god();
	int  think_god_attack_target(int& targetXLoc, int& targetYLoc);

#ifdef AMPLUS
	void think_egyptian_god();
	void think_indian_god();
	void think_zulu_god();
#endif
	*/
};

//--------------------------------------------//

#endif

