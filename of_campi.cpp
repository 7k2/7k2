//Filename   : OF_CAMPI.CPP
//Description: Interface for Camp

#include <OVGA.H>
#include <OF_CAMP.H> 
#include <ALL.H>
#include <ORACERES.H>
#include <OMONSRES.H>
#include <OHERORES.H>
#include <OSYS.H>
#include <OMODEID.H>
#include <OFONT.H>
#include <OMOUSE.H>
#include <OIMGRES.H>
#include <OREMOTE.H>
#include <OSE.H> 
#include <OHELP.H>
#include <OTRANSL.H>
#include <OBUTT3D.H>
#include <OCONFIG.H>
#include <ONATION.H>
#include <OUNIT.H>
#include <OSPY.H>
#include <OITEMRES.H>
#include <OBITMAP.H>
#include <OSERES.H>
#include <OT_FIRM.H>
#include <OT_UNIT.H>

enum	{PATROL_ALL=0,
		 PATROL_NO_GENERAL=1,
		 PATROL_NO_INJURED_SOILDER=2};

//----------- Define static vars -------------//

static patrol_state = PATROL_ALL;
static char	last_menu_mode;
static Button3D button_patrol, button_reward, button_defense, button_camp_upgrade;
static Button3D button_promote;
static short  	 pop_disp_y1;
static int      disp_soldier_list_y1;
static int      disp_soldier_info_y1;
static char		 disp_combat_or_skill;	// 0=normal, 1=combat, 2=skill, 3=spying, 4=loyalty
static char		 swap_item_src;
static char		 pointed_soldier_id;
short FirmCamp::swap_item_id;

static short edit_hp_x1, edit_hp_y1, edit_hp_x2, edit_hp_y2, edit_hp_enable;
static short edit_loyalty_x1, edit_loyalty_y1, edit_loyalty_x2, edit_loyalty_y2, edit_loyalty_enable;
static short edit_combat_x1, edit_combat_y1, edit_combat_x2, edit_combat_y2, edit_combat_enable;
static short edit_max_combat_x1, edit_max_combat_y1, edit_max_combat_x2, edit_max_combat_y2, edit_max_combat_enable;
static short edit_skill_x1, edit_skill_y1, edit_skill_x2, edit_skill_y2, edit_skill_enable;
static short edit_max_skill_x1, edit_max_skill_y1, edit_max_skill_x2, edit_max_skill_y2, edit_max_skill_enable;
static short edit_spy_x1, edit_spy_y1, edit_spy_x2, edit_spy_y2, edit_spy_enable;
static short clear_spy_x1, clear_spy_y1, clear_spy_x2, clear_spy_y2, clear_spy_enable;
static char add_spy_str[MAX_NATION+1], add_spy_nation[MAX_NATION+1];
static short add_spy_x1, add_spy_y1, add_spy_x2, add_spy_y2;


//--------- Declare static functions ---------//

static void disp_debug_info(FirmCamp* firmPtr, int refreshFlag);
static void disp_soldier_hit_points(int x1, int y1, int x2, int hitPoints, int maxHitPoints);
static void disp_training_bar(int x1, int y1, int x2, int hitPoints, int maxHitPoints);

#define SOLDIER_PER_ROW      8
#define SOLDIER_X_SPACING   24
#define SOLDIER_Y_SPACING   86


//--------- Begin of function FirmCamp::put_info ---------//
//
void FirmCamp::put_info(int refreshFlag)
{
	// ##### begin Gilbert 21/9 ######//
	if( refreshFlag == INFO_REPAINT )
	{
		last_menu_mode = firm_menu_mode = FIRM_MENU_MAIN;
		disp_combat_or_skill = 0;		// display combat
	}
	// ##### end Gilbert 21/9 ######//
	else
	{
		if( last_menu_mode != firm_menu_mode )		// if changing menu mode pass repaint to sub-menu
		{
			refreshFlag = INFO_REPAINT;
			last_menu_mode = firm_menu_mode;
		}
	}

	Firm::put_info(refreshFlag);

	switch( firm_menu_mode )
	{
	case FIRM_MENU_MAIN:
		if( should_show_info() )
		{
			vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("FORTBASE") );

			disp_camp_info(INFO_Y1, refreshFlag);
			Firm *firmPtr = firm_array[firm_recno];

			if( firm_id == FIRM_CAMP )
				disp_camp_upgrade(INFO_Y1, refreshFlag);

			disp_soldier_list(INFO_Y1, refreshFlag, 0);
			disp_soldier_info(INFO_Y1+178, refreshFlag);
			// disp_debug_info(this, refreshFlag);
		}
		break;

	case FIRM_MENU_TRAIN:
		vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("FORTBASE") );
		disp_train_menu(refreshFlag);
		break;

	case FIRM_MENU_SPY:
	case FIRM_MENU_SELECT_BRIBER:
		vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("FORTBASE") );
		disp_spy_menu(refreshFlag);
		break;

	case FIRM_MENU_SET_BRIBE_AMOUNT:
//		vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("FORTBASE") );
		vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("BLDGBASE") );
		disp_bribe_menu(refreshFlag);
		break;

	case FIRM_MENU_BRIBE_RESULT:
//		vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("FORTBASE") );
		vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("BLDGBASE") );
		disp_bribe_result(refreshFlag);
		break;

	case FIRM_MENU_VIEW_SECRET:
		spy_array.disp_view_secret_menu(action_spy_recno, refreshFlag);
		break;

	case FIRM_MENU_ASSASSINATE_CONFIRM:
		vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("BLDGBASE") );
		disp_assassinate_confirm(refreshFlag);
		break;

	case FIRM_MENU_ASSASSINATE_RESULT:
		vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("BLDGBASE") );
		disp_assassinate_result(refreshFlag);
		break;

	case FIRM_MENU_STEAL_TECH_CONFIRM:
		vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("BLDGBASE") );
		disp_steal_tech_confirm(refreshFlag);
		break;

	case FIRM_MENU_STEAL_TECH_RESULT:
		vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("BLDGBASE") );
		disp_steal_tech_result(refreshFlag);
		break;

	case FIRM_MENU_INCIDENT_NATION:
		vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("BLDGBASE") );
		disp_incident_nation(refreshFlag);
		break;

	case FIRM_MENU_INCIDENT_CONFIRM:
		vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("BLDGBASE") );
		disp_incident_confirm(refreshFlag);
		break;

	case FIRM_MENU_INCIDENT_RESULT:
		vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("BLDGBASE") );
		disp_incident_result(refreshFlag);
		break;

	default:
		err_when( firm_menu_mode < FIRM_MENU_CAMP_LAST );
		// if firm_menu_mode >= FIRM_MENU_CAMP_LAST, handled by sub-class
	}
}
//----------- End of function FirmCamp::put_info -----------//


//--------- Begin of function FirmCamp::detect_info ---------//
//
void FirmCamp::detect_info()
{
	Firm::detect_info();

	switch( firm_menu_mode )
	{
	case FIRM_MENU_MAIN:
		if( should_show_info() )
		{
			detect_camp_info();
			detect_soldier_list(0);
			detect_soldier_info();
			detect_spy_button();
		}
		break;

	case FIRM_MENU_TRAIN:
		detect_train_menu();
		break;

	case FIRM_MENU_SPY:
	case FIRM_MENU_SELECT_BRIBER:
		detect_spy_menu();
		break;

	case FIRM_MENU_SET_BRIBE_AMOUNT:
		detect_bribe_menu();
		break;

	case FIRM_MENU_BRIBE_RESULT:
		detect_bribe_result();
		break;

	case FIRM_MENU_VIEW_SECRET:
		if( spy_array.detect_view_secret_menu(action_spy_recno, nation_recno) )
		{
			firm_menu_mode = FIRM_MENU_MAIN;
//			info.disp();
		}
		break;

	case FIRM_MENU_ASSASSINATE_CONFIRM:
		detect_assassinate_confirm();
		break;

	case FIRM_MENU_ASSASSINATE_RESULT:
		detect_assassinate_result();
		break;

	case FIRM_MENU_STEAL_TECH_CONFIRM:
		detect_steal_tech_confirm();
		break;

	case FIRM_MENU_STEAL_TECH_RESULT:
		detect_steal_tech_result();
		break;

	case FIRM_MENU_INCIDENT_NATION:
		detect_incident_nation();
		break;

	case FIRM_MENU_INCIDENT_CONFIRM:
		detect_incident_confirm();
		break;

	case FIRM_MENU_INCIDENT_RESULT:
		detect_incident_result();
		break;

	default:
		err_when( firm_menu_mode < FIRM_MENU_CAMP_LAST );
		// if firm_menu_mode >= FIRM_MENU_CAMP_LAST, handled by sub-class
	}
}
//----------- End of function FirmCamp::detect_info -----------//


//--------- Begin of function FirmCamp::disp_camp_upgrade ---------//
//
void FirmCamp::disp_camp_upgrade(int dispY1, int refreshFlag)
{
	if( is_own() )
	{
		int x2 = INFO_X1 +13;
		int y2 = INFO_Y1 +281;
		char iconName[]="CTOF-00";
	
		if( refreshFlag==INFO_REPAINT )
		{
			button_camp_upgrade.create( x2 + 162, y2, 'A', iconName );
			button_camp_upgrade.set_help_code( "CAMPTOFORT" );
		}
		button_camp_upgrade.enable_flag = 1;

		if ( upgrading_firm_id )
		{
			iconName[5] = '0' + upgrade_completion_percent/40;
			iconName[6] = '0' + upgrade_completion_percent/4 % 10;
			vga.active_buf->put_bitmap( x2 +162, y2, image_spict.read(iconName) );
		}
		else
			button_camp_upgrade.paint();
	}
}
//----------- End of function FirmCamp::disp_camp_upgrade -----------//

//--------- Begin of function FirmCamp::disp_camp_info ---------//
//
void FirmCamp::disp_camp_info(int dispY1, int refreshFlag)
{
	if( is_own() )
	{
		int x1 = INFO_X1 +13;
		int y1 = INFO_Y1 +235;
		int x2 = INFO_X1 +13;
		int y2 = INFO_Y1 +281;
		if( refreshFlag==INFO_REPAINT )
		{
			// ##### begin Gilbert 31/12 #######//
			// button_patrol.create( INFO_X1+13, INFO_Y1+235, 'A', "PATROL" );
			// button_reward.create( INFO_X1+13+BUTTON_DISTANCE, INFO_Y1+235, 'A', "REWARDCB" );
			// button_defense.create( INFO_X1+13+2*BUTTON_DISTANCE, INFO_Y1+235, 'A', defense_flag ? "DEFENSE1" : "DEFENSE0" );

			if (!is_monster())
				button_patrol.create( INFO_X1+13+BUTTON_DISTANCE, INFO_Y1+281, 'A', "PATROL" );
			else
				button_patrol.create( INFO_X1+13+BUTTON_DISTANCE, INFO_Y1+281, 'A', "F_PATROL" );

			if (!is_monster())
				button_reward.create( INFO_X1+13, INFO_Y1+235, 'A', "REWARD" );				
			else
				button_reward.create( INFO_X1+13, INFO_Y1+235, 'A', "F_REWARD" );				
							
			if (!is_monster())
				button_defense.create( INFO_X1+13+2*BUTTON_DISTANCE, INFO_Y1+281, 'A', defense_flag ? "DEFENSE1" : "DEFENSE0" );
			else
				button_defense.create( INFO_X1+13+2*BUTTON_DISTANCE, INFO_Y1+281, 'A', defense_flag ? "F_DEFEN1" : "F_DEFEN0" );
				
			if (!is_monster())
				button_promote.create( INFO_X1+13+2*BUTTON_DISTANCE , INFO_Y1+235, 'A', "PROMOTE" );
			else
				button_promote.create( INFO_X1+13+2*BUTTON_DISTANCE , INFO_Y1+235, 'A', "F_PROMOT" );
			// ##### end Gilbert 31/12 #######//
		}
		if( overseer_recno )
		{
			button_patrol.enable_flag = 1;
		}
		else
		{
			for( int i = 0; i < soldier_count && soldier_array[i].is_under_training(); ++i );
			button_patrol.enable_flag = i < soldier_count;
		}
		
		String str;
		switch(patrol_state)
		{
			case PATROL_ALL:
			//	str = "Sortie All";
				str = "";
				break;
			case PATROL_NO_GENERAL:
				str = text_firm.str_patrol_state(1); // "Sortie No Leader";
				break;
			case PATROL_NO_INJURED_SOILDER:
				str = text_firm.str_patrol_state(2); // "Sortie No Injured";
				break;
			default:
				break;
		}

		button_patrol.paint();
	//	vga.active_buf->bar_alpha( button_patrol.x1, button_patrol.y1+15, button_patrol.x1+BUTTON_ACTION_WIDTH-1, button_patrol.y1+BUTTON_ACTION_HEIGHT-16, 1, 0 );
		font_whbl.center_put_paragraph( button_patrol.x1, button_patrol.y1, button_patrol.x1+BUTTON_ACTION_WIDTH-1, button_patrol.y1+BUTTON_ACTION_HEIGHT-1, str );

		// ###### begin Gilbert 15/4 ########//
		if( nation_array[nation_recno]->cash >= REWARD_COST &&
			 ( (overseer_recno && unit_array[overseer_recno]->rank_id != RANK_KING && unit_res[unit_array[overseer_recno]->unit_id]->class_info.has_loyalty)
			  || (selected_soldier_id && selected_soldier_id <= soldier_count && unit_res[soldier_array[selected_soldier_id-1].unit_id]->class_info.has_loyalty)) )
			button_reward.enable_flag = 1;			// call paint
		// ###### end Gilbert 15/4 ########//
		else
			button_reward.enable_flag = 0;		// call paint
		button_reward.paint();

		if (!is_monster())
			button_defense.update_bitmap( defense_flag ? "DEFENSE1" : "DEFENSE0" );		// call paint
		else
			button_defense.update_bitmap( defense_flag ? "F_DEFEN1" : "F_DEFEN0" );

		if( (button_promote.visible_flag = !overseer_recno) )
		{
			button_promote.enable_flag = 0;
			if( selected_soldier_id > 0 && selected_soldier_id <= soldier_count )
			{
				Soldier *soldierPtr = soldier_array + selected_soldier_id - 1;
				// ##### begin Gilbert 24/3 ######//
				err_when( soldierPtr->unit_id == UNIT_WAGON );

				if( soldierPtr->race_id != 0
					&& soldierPtr->rank_id == RANK_SOLDIER 
					&& !soldierPtr->is_under_training()
					&& soldierPtr->skill_level() > 0 )
				// ##### end Gilbert 24/3 ######//
				{
					button_promote.enable_flag = 1;
				}
			}

			button_promote.paint();
		}
	}
	disp_spy_button( INFO_X1+13+BUTTON_DISTANCE, INFO_Y1+281, refreshFlag);
}
//----------- End of function FirmCamp::disp_camp_info -----------//


//--------- Begin of function FirmCamp::detect_camp_info ---------//
//
void FirmCamp::detect_camp_info()
{
	int rc;
	if( is_own() )
	{
		//----------- detect patrol -----------//

		if( (rc = button_patrol.detect(0, 0, 1)) )
		{
			if (rc == 2)
			{
				patrol_state = (++patrol_state) % 3;
			}
			else
			if(remote.is_enable())
			{
				// packet structure : <firm recno> <patrol_state>
				short *shortPtr=(short *)remote.new_send_queue_msg(MSG_F_CAMP_PATROL, 2 * sizeof(short));
				shortPtr[0] = firm_recno;
				shortPtr[1] = patrol_state;
			}
			else
			{
				patrol(patrol_state);
			}
			se_ctrl.immediate_sound("SORTIE");
			if( overseer_recno )
			{
				Unit *overseer = unit_array[overseer_recno];
				se_res.sound( center_x, center_y, 1, 'S', overseer->sprite_id, "ACK");
			}
			else
			if( soldier_count > 0 )
			{
				Soldier *soldierPtr = &soldier_array[0];
				int unitId = soldierPtr->unit_id;
				UnitInfo *unitInfo = unit_res[unitId];
				se_res.sound( center_x, center_y, 1, 'S', unitInfo->sprite_id, "ACK");
			}				
		}

		//----------- detect reward -----------//

		if( button_reward.detect() )
		{
			reward(selected_soldier_id, COMMAND_PLAYER);
			se_ctrl.immediate_sound("TURN_ON");
		}

		if( firm_id==FIRM_CAMP && button_camp_upgrade.detect() )
		{
			if (!upgrading_firm_id)
			{
				se_ctrl.immediate_sound("TURN_ON");
				start_upgrade(FIRM_FORT, COMMAND_PLAYER);	
			}
		}

		//----- detect defense mode button -------//

		if( button_defense.detect() )
		{
			se_ctrl.immediate_sound( !defense_flag?"TURN_ON":"TURN_OFF");

			if( !remote.is_enable() )
			{
				// update RemoteMsg::toggle_camp_patrol()
				defense_flag = !defense_flag;
			}
			else
			{
				// packet structure : <firm recno> <defense_flag>
				short *shortPtr=(short *)remote.new_send_queue_msg(MSG_F_CAMP_TOGGLE_PATROL, 2*sizeof(short));
				shortPtr[0] = firm_recno;
				shortPtr[1] = !defense_flag;
			}

//			button_defense.update_bitmap( defense_flag ? "DEFENSE1" : "DEFENSE0" );
		}

		if( button_promote.detect() )
		{
			promote_soldier( selected_soldier_id, COMMAND_PLAYER );
			se_ctrl.immediate_sound("PROMOTE");
		}
	}
}
//----------- End of function FirmCamp::detect_camp_info -----------//



//--------- Begin of function FirmCamp::disp_soldier_list ---------//
//
void FirmCamp::disp_soldier_list(int dispY1, int refreshFlag, int dispSpyMenu)
{
	disp_soldier_list_y1 = dispY1;

	if( refreshFlag == INFO_REPAINT )
	{
		swap_item_src = -1;		// reset from selecting swap target
	}
	pointed_soldier_id = -1;

	for( int inc = -1; inc <= 1; inc += 2 )
	{
		err_when( inc == 0 );

		// first round is descending draw to icon
		// second round is ascending to draw the frame

		int inAreaFlag = 4;

		for( int i = inc>=0?0:soldier_count; i >= 0 && i <= soldier_count; i +=inc )
		{
			// display soldier i

			int row = i/SOLDIER_PER_ROW;
			int x = INFO_X1 + 18 + (i % SOLDIER_PER_ROW) * SOLDIER_X_SPACING;
			int y = INFO_Y1 + 50 + row * SOLDIER_Y_SPACING;
			int yHp = INFO_Y1 + 7 + row * SOLDIER_Y_SPACING;

			int windowX1 = INFO_X1 + 16;
			int windowX2 = INFO_X1 + 220;
			int windowY1 = INFO_Y1 + 5 + row * 84;	// 5,89
			int windowY2 = windowY1 + 80 - 1 ;

			int unitId;
			int hp;
			int maxHp;
			// ##### begin Gilbert 21/9 ######//
			int combatLevel;
			int skillLevel;
			int loyalty;
			// ##### end Gilbert 21/9 ######//
			int ownSpy;
			int itemId;

			if( i==0 )
			{
				if( !overseer_recno )
					continue;

				// overseer
				Unit *overseer = unit_array[overseer_recno];
				unitId = overseer->unit_id;

				hp = (int) overseer->hit_points;
				maxHp = overseer->max_hit_points();
				combatLevel = (int) overseer->combat_level();
				skillLevel = (int) overseer->skill_level();

				if( overseer->rank_id != RANK_GENERAL )
					loyalty = -1;		// king or other(?)
				else
					loyalty = overseer->loyalty;

				ownSpy = overseer->is_own_spy() ? overseer->spy_recno : 0;
				itemId = overseer->item.id;
			}
			else
			{
				// soldier
				Soldier *soldierPtr = &soldier_array[i-1];
				unitId = soldierPtr->unit_id;

				hp = soldierPtr->hit_points;
				maxHp = soldierPtr->max_hit_points();
				combatLevel = (int) soldierPtr->combat_level();
				skillLevel = (int) soldierPtr->skill_level();

				// ####### begin Gilbert 24/3 #########//
				if( unit_res[soldierPtr->unit_id]->class_info.has_loyalty && nation_recno )
				// ####### end Gilbert 24/3 #########//
					loyalty = soldierPtr->loyalty;
				else
					loyalty = -1;
				ownSpy = soldierPtr->is_own_spy() ? soldierPtr->spy_recno : 0;
				itemId = soldierPtr->item.id;
			}

			if( dispSpyMenu && !ownSpy )			// skip displaying spy
				continue;

			UnitInfo *unitInfo = unit_res[unitId];

			// display that solider icon at x+SOLDIER_X_SPACING/2, y
			// draw a frame if selected

			if( inc < 0 )
			{
				// first round is descending draw to icon
				// Soldier *soldierPtr = &soldier_array[i-1];
				info.draw_unit_icon( x+SOLDIER_X_SPACING/2, y,
					unitId, nation_recno,
					windowX1, windowY1, windowX2, windowY2,
					(combatLevel < BASIC_COMBAT_TRAIN) ? (((BASIC_COMBAT_TRAIN - combatLevel) <<6)+ 33) : 1);
			}
			else
			{
				// second round is ascending to draw the frame

				if( info.draw_unit_icon( x+SOLDIER_X_SPACING/2, y,
					unitId, nation_recno,
					windowX1, windowY1, windowX2, windowY2, 
					inAreaFlag | (i==selected_soldier_id?3:0) ) & 4 )
				{
					inAreaFlag = 0;		// frame for mouse cursor is drawn, disable the frame
					pointed_soldier_id = i;
				}

				// display combat skill

				// ######## begin Gilbert 21/9 #######//
				Font *font = &font_whbl;

				if( !dispSpyMenu )
				{
					if( disp_combat_or_skill )		// display skill level
					{
						font = &font_blue;
						int attribute = -1;
						switch( disp_combat_or_skill )
						{
						case 1: 
							if( unitInfo->class_info.has_combat_level )
								attribute = combatLevel;
							break;
						case 2:
							if( unitInfo->class_info.has_skill_level && skillLevel > 0 )
								attribute = skillLevel; 
							break;
						case 4:
							if( unitInfo->class_info.has_loyalty && nation_recno )
								attribute = loyalty;
							break;
						default:
							err_here();
						}
						if( attribute >= 0 )	// hide attribute on some cases
							font->center_put( x, yHp, x+SOLDIER_X_SPACING, 
								yHp+font->max_font_height, m.format(attribute) );
					}
					else if( ownSpy )		// display spy icon
					{
						vga.active_buf->put_bitmap_trans( x+SOLDIER_X_SPACING/2-8, yHp-5, image_icon.read("U_SPY") );
					}
					else if( unitInfo->class_info.has_combat_level ) 	// display combat skill
					{
						font->center_put( x, yHp, x+SOLDIER_X_SPACING, yHp+font->max_font_height, 
							m.format(combatLevel) );
					}

					if( itemId )
					{
						char *iconPtr = item_res.item_unit_interface(itemId);
						if( iconPtr )
							vga.active_buf->put_bitmap_trans( x+SOLDIER_X_SPACING/2-((Bitmap *)iconPtr)->get_width()/2,
							yHp +53, iconPtr );
					}
				}
				else
				{
					// display spy skill
					err_when( !ownSpy );
					font_whbl.center_put( x, yHp, x+SOLDIER_X_SPACING, yHp+font->max_font_height, 
						m.format(spy_array[ownSpy]->spy_skill) );
				}

				// display hit points bar

				if( i > 0 && soldier_array[i-1].is_under_training() )
					disp_training_bar( x, yHp+font->max_font_height+2, x+SOLDIER_X_SPACING*7/8-1,
						soldier_array[i-1].skill_level(), BASIC_COMBAT_TRAIN );
				else
					disp_soldier_hit_points( x, yHp+font->max_font_height+2, x+SOLDIER_X_SPACING*7/8-1, hp, maxHp );
			}
		}
	}

/*
	//---------------- paint the panel --------------//

	if( overseer_recno )
	{
		//------------ display overseer info -------------//

		Unit* overseerUnit = unit_array[overseer_recno];

		int x=INFO_X1+6, y=dispY1+4, x1=x+UNIT_LARGE_ICON_WIDTH+8;

		if( selected_soldier_id == 0 )
		{
			vga_front.rect( x-2, y-2, x+UNIT_LARGE_ICON_WIDTH+1, y+UNIT_LARGE_ICON_HEIGHT+1, 2, V_YELLOW );
		}
		else
		{
			vga.blt_buf( x-2, y-2, x+UNIT_LARGE_ICON_WIDTH+1, y-1, 0 );
			vga.blt_buf( x-2, y+UNIT_LARGE_ICON_HEIGHT+1, x+UNIT_LARGE_ICON_WIDTH+1, y+UNIT_LARGE_ICON_HEIGHT+2, 0 );
			vga.blt_buf( x-2, y-2, x-1, y+UNIT_LARGE_ICON_HEIGHT+2, 0 );
			vga.blt_buf( x+UNIT_LARGE_ICON_WIDTH, y-2, x+UNIT_LARGE_ICON_WIDTH+1, y+UNIT_LARGE_ICON_HEIGHT+2, 0 );
		}

		//-------------------------------------//

		if( refreshFlag == INFO_REPAINT )
		{
			vga_front.put_bitmap(x, y, unit_res[overseerUnit->unit_id]->get_large_icon_ptr(overseerUnit->rank_id) );
		}

		//-------- set help parameters --------//

		if( mouse.in_area(x, y, x+UNIT_LARGE_ICON_WIDTH+3, y+UNIT_LARGE_ICON_HEIGHT+3) )
			help.set_unit_help( overseerUnit->unit_id, overseerUnit->rank_id, x, y, x+UNIT_LARGE_ICON_WIDTH+3, y+UNIT_LARGE_ICON_HEIGHT+3);

		//-------------------------------------//

		if( overseerUnit->rank_id == RANK_KING )
		{
			if( refreshFlag == INFO_REPAINT )
				font_san.put( x1, y, "King" );

			y+=14;
		}

		if( refreshFlag == INFO_REPAINT )
			font_san.put( x1, y, overseerUnit->unit_name(0), 0, INFO_X2-2 );		// 0-ask unit_name() not to return the title of the unit

		y+=14;

		//------- display leadership -------//

		String str;

		str  = translate.process("Leadership");
		str += ": ";
		str += overseerUnit->skill.skill_level;

		font_san.disp( x1, y, str, INFO_X2-10 );
		y+=14;

		//--------- display loyalty ----------//

		if( overseerUnit->rank_id != RANK_KING )
		{
			x1 = font_san.put( x1, y, "Loyalty:" );

			int x2 = info.disp_loyalty( x1, y-1, x1, overseerUnit->loyalty, overseerUnit->target_loyalty, nation_recno, refreshFlag );

			if( overseerUnit->spy_recno )
			{
				//------ if this is the player's spy -------//

				if( overseerUnit->is_own_spy() )
				{
					vga_front.put_bitmap( x2+5, y+1, image_icon.get_ptr("U_SPY") );
					x2 += 15;
				}
			}

			vga.blt_buf( x2, y-1, INFO_X2-2, dispY1+44, 0 );
		}
	}

	pop_disp_y1 = dispY1;

	//---------------- paint the panel --------------//

	if( refreshFlag == INFO_REPAINT )
		vga.d3_panel_up( INFO_X1, dispY1, INFO_X2, dispY1+60 );

	//----------- display populatin distribution ---------//

	int overseerRaceId=0;

	if( overseer_recno )
		overseerRaceId = unit_array[overseer_recno]->race_id;

	if( selected_soldier_id > soldier_count )
		selected_soldier_id = soldier_count;

	//------ display population composition -------//

	int	  x, y;
	Soldier* soldierPtr = soldier_array;
	static  char last_race_id_array[MAX_SOLDIER];
	static  char last_unit_id_array[MAX_SOLDIER];

	dispY1+=1;

	for( int i=0 ; i<MAX_SOLDIER ; i++, soldierPtr++ )
	{
		x = INFO_X1+4+i%4*50;
		y = dispY1+i/4*29;

		if( i<soldier_count )
		{
			if( refreshFlag==INFO_REPAINT ||
				 last_race_id_array[i] != soldierPtr->race_id ||
				 last_unit_id_array[i] != soldierPtr->unit_id )
			{
				vga_front.put_bitmap(x+2, y+2, soldierPtr->small_icon_ptr());
			}

			//----- highlight the selected soldier -------//

			if( selected_soldier_id == i+1 )
				vga_front.rect( x, y, x+27, y+23, 2, V_YELLOW );
			else
				vga_front.rect( x, y, x+27, y+23, 2, vga_front.color_up );

			//------ display hit points bar --------//

			disp_soldier_hit_points( x+2, y+24, x+25, soldierPtr->shown_hit_points(), soldierPtr->max_hit_points() );

			//----- display combat or skill level ------//

			char* spyIconName=NULL;

			if( soldierPtr->spy_recno )
			{
				Spy* spyPtr = spy_array[soldierPtr->spy_recno];

				//------ if this is the player's spy -------//

				if( nation_array.player_recno &&
					 spyPtr->true_nation_recno == nation_array.player_recno )
				{
					spyIconName = "U_SPY";
				}

				//--------------------------------------------//
				//
				// If this is an enemy spy and this firm belongs
				// to the player and there is a player's phoenix
				// over this firm and the spying skill of the spy
				// is low (below 40)
				//
				//--------------------------------------------//

//				else if( spyPtr->spy_skill < 40 &&
//							nation_recno == nation_array.player_recno &&
//							nation_array.player_recno &&
//					 (~nation_array)->revealed_by_phoenix(loc_x1, loc_y1) )
//				{
//					spyIconName = "ENEMYSPY";
//				}

			}

			//--------------------------------------//

			if( spyIconName )
			{
				vga_front.put_bitmap( x+30, y+6, image_icon.get_ptr(spyIconName) );
				vga.blt_buf( x+40, y+6, x+49, y+15, 0 );
				vga.blt_buf( x+30, y+16, x+49, y+26, 0 );
			}
			else
			{
				font_san.disp(x+30, y+6, soldierPtr->skill.combat_level, 1, x+49);
			}

			last_race_id_array[i] = soldierPtr->race_id;
			last_unit_id_array[i] = soldierPtr->unit_id;

			//------- set help parameters ---------//

			if( mouse.in_area(x, y, x+27, y+23) )
				help.set_unit_help( soldierPtr->unit_id, 0, x, y, x+27, y+23 );
		}
		else
		{
			if( last_race_id_array[i] != 0 || last_unit_id_array[i] != 0 )
			{
				vga.blt_buf( x, y, x+49, y+27, 0 );
				last_race_id_array[i] = 0;
				last_unit_id_array[i] = 0;
			}
		}
	}
*/
}
//----------- End of function FirmCamp::disp_soldier_list -----------//


//--------- Begin of function FirmCamp::detect_soldier_list ---------//
//
// <int> selecteSpyMenu        0=main menu; 1=selecting spy
// when selectSpyMenu is 0, return 1 if left click on a unit, return 2 if right click on a unit
// when selectSpyMenu is 1, return spy_recno of the clicked spy, 0 if no own spy is clicked
//
int FirmCamp::detect_soldier_list(int selectSpyMenu)
{
	int dispY1 = disp_soldier_list_y1;

	// display in ascending order to select the overseer first

	for( int i = 0; i <= soldier_count; ++i )
	{
		// display soldier i

		int row = i/SOLDIER_PER_ROW;
		int x = INFO_X1 + 18 + (i % SOLDIER_PER_ROW) * SOLDIER_X_SPACING;
		int y = INFO_Y1 + 50 + row * SOLDIER_Y_SPACING;
		int yHp = INFO_Y1 + 7 + row * SOLDIER_Y_SPACING;

		int windowX1 = INFO_X1 + 16;
		int windowX2 = INFO_X1 + 220;
		int windowY1 = INFO_Y1 + 5 + row * 84;	// 5,89
		int windowY2 = windowY1 + 80 - 1 ;

		int unitId;
		int hp;
		int maxHp;
		// ##### begin Gilbert 21/9 ######//
		int combatLevel;
		int skillLevel;
		int loyalty;
		// ##### end Gilbert 21/9 ######//
		int ownSpy;
		int itemId;

		if( i==0 )
		{
			if( !overseer_recno )
				continue;

			// overseer
			Unit *overseer = unit_array[overseer_recno];
			unitId = overseer->unit_id;

			hp = (int) overseer->hit_points;
			maxHp = overseer->max_hit_points();
			combatLevel = (int) overseer->combat_level();
			skillLevel = (int) overseer->skill_level();

			if( overseer->rank_id != RANK_GENERAL )
				loyalty = overseer->loyalty;
			else
				loyalty = -1;		// king or other(?)

			ownSpy = overseer->is_own_spy() ? overseer->spy_recno : 0;
			itemId = overseer->item.id;
		}
		else
		{
			// soldier
			Soldier *soldierPtr = &soldier_array[i-1];
			unitId = soldierPtr->unit_id;

			hp = soldierPtr->hit_points;
			maxHp = soldierPtr->max_hit_points();
			combatLevel = (int) soldierPtr->combat_level();
			skillLevel = (int) soldierPtr->skill_level();

			if( soldierPtr->race_id )
				loyalty = soldierPtr->loyalty;
			else
				loyalty = -1;

			ownSpy = soldierPtr->is_own_spy() ? soldierPtr->spy_recno : 0;
			itemId = soldierPtr->item.id;
		}

		if( selectSpyMenu && !ownSpy )
			continue;

		int rc = info.draw_unit_icon( x+SOLDIER_X_SPACING/2, y,
			unitId, nation_recno, 
			windowX1, windowY1, windowX2, windowY2, 24 );		// detect left button (8) and right button(16)

		if( !rc )
			continue;

		if( selectSpyMenu == 0 )
		{
			// -------- main menu ---------//

			if( rc & 8 )
			{
				// ----- left click select soldier/overseer -------//

				selected_soldier_id = i;
				swap_item_src = selected_soldier_id;		// turn on selecting swap target
				swap_item_id = itemId;
				return 1;
			}
			else if( rc & 16 && is_own() )
			{
				// ------ right click mobilize solidier/overseer ------//

				if( i == 0 )
				{
					if(remote.is_enable())
					{
						// packet structure : <firm recno>
						short *shortPtr=(short *)remote.new_send_queue_msg(MSG_FIRM_MOBL_OVERSEER, sizeof(short));
						shortPtr[0] = firm_recno;
					}
					else
					{
						assign_overseer(0);		// the overseer quits the camp
					}
				}
				else
				{
					// #### begin Gilbert 26/1 #####//
					if( !soldier_array[i-1].is_under_training() )
						mobilize_soldier(i, COMMAND_PLAYER);
					else
						cancel_train_soldier(i, COMMAND_PLAYER);
					// #### end Gilbert 26/1 #####//
				}
				return 2;
			}
		}
		else if( selectSpyMenu == 1 )
		{
			if( rc & 8 && ownSpy )
			{
				selected_soldier_id = i;
				return ownSpy;
			}
		}
	}

	// swaping item

	if( is_own() && selectSpyMenu == 0 && swap_item_src >= 0 && mouse.release_click() )
	{
		if( pointed_soldier_id >= 0 )
		{
			swap_item( swap_item_src, pointed_soldier_id, swap_item_id, COMMAND_PLAYER );
		}
		else
		{
			swap_item_src = -1;		// reset from selecting swap target
		}
	}

	return 0;

	/*
	if( !should_show_info() )
		return 0;

	if( is_own() )
	{
		//------ detect the overseer button -----//
		int rc = mouse.single_click(INFO_X1+6, INFO_Y1+58,
					INFO_X1+5+UNIT_LARGE_ICON_WIDTH, INFO_Y1+57+UNIT_LARGE_ICON_HEIGHT, 2 );

		if( rc==1 )
		{
			selected_soldier_id = 0;
			return 1;
		}
		else if( rc==2 && is_own() )
		{
			if(remote.is_enable())
			{
				// packet structure : <firm recno>
				short *shortPtr=(short *)remote.new_send_queue_msg(MSG_FIRM_MOBL_OVERSEER, sizeof(short));
				shortPtr[0] = firm_recno;
			}
			else
			{
				assign_overseer(0);		// the overseer quits the camp
			}
			return 1;
		}
	}

	//------- detect buttons on hiring firm soldiers -------//

	int i, x, y;

	for( i=0 ; i<soldier_count ; i++ )
	{
		x = INFO_X1+6+i%4*50;
		y = pop_disp_y1+1+i/4*29;

		switch( mouse.single_click(x, y, x+27, y+23, 2) )
		{
		case 1:         // left button to select soldier
			selected_soldier_id = i+1;
			return 1;

		case 2:
			if( is_own() )		// only if this is our own firm
			{
				//--- if the town where the unit lives belongs to the nation of this firm ---//

				mobilize_soldier(i+1, COMMAND_PLAYER);
				return 1;
			}
			break;
		}
	}

	return 0;
	*/
}
//----------- End of function FirmCamp::detect_soldier_list -----------//


//--------- Begin of function disp_soldier_hit_points ---------//
//
static void disp_soldier_hit_points(int x1, int y1, int x2, int hitPoints, int maxHitPoints)
{
	//------- determine the hit bar type -------//

	#define HIT_BAR_TYPE_COUNT  3

	int  hit_bar_color_array[HIT_BAR_TYPE_COUNT] = { 0xA8, 0xB4, 0xAC };
	int  hit_bar_max_array[HIT_BAR_TYPE_COUNT] 	= { 50, 100, 200 };
	char hitBarColor;

	for( int i=0 ; i<HIT_BAR_TYPE_COUNT ; i++ )
	{
		if( maxHitPoints <= hit_bar_max_array[i] || i==HIT_BAR_TYPE_COUNT-1 )
		{
			hitBarColor = hit_bar_color_array[i];
			break;
		}
	}

	//------- draw the hit points bar -------//

	enum { HIT_BAR_DARK_BORDER = 3,
			 HIT_BAR_BODY 		   = 1 };

	int barWidth = (x2-x1+1) * hitPoints / max(hitPoints, maxHitPoints);

	vga.active_buf->bar( x1, y1, x1+barWidth-1, y1+1, hitBarColor + HIT_BAR_BODY );

//	if( x1+barWidth <= x2 )
//		vga.blt_buf( x1+barWidth, y1, x2, y1+1, 0 );

	y1+=2;

	vga.active_buf->bar( x1, y1, x1+barWidth-1, y1, hitBarColor + HIT_BAR_DARK_BORDER );
	vga.active_buf->bar( x1+barWidth, y1, x1+barWidth, y1, V_BLACK );

//	if( x1+barWidth+1 <= x2 )
//		vga.blt_buf( x1+barWidth+1, y1, x2, y1, 0 );

	y1++;

	vga.active_buf->bar( x1+1, y1, x1+barWidth, y1, V_BLACK );

//	if( x1+barWidth+1 <= x2 )
//		vga.blt_buf( x1+barWidth+1, y1, x2, y1, 0 );
}
//----------- End of function disp_soldier_hit_points -----------//


//--------- Begin of function disp_training_bar ---------//
//
static void disp_training_bar(int x1, int y1, int x2, int hitPoints, int maxHitPoints)
{
	//------- determine the hit bar type -------//

	int hitBarColor = 0xa4;	// blue

	//------- draw the hit points bar -------//

	enum { HIT_BAR_DARK_BORDER = 3,
			 HIT_BAR_BODY 		   = 1 };

	int barWidth = (x2-x1+1) * hitPoints / max(hitPoints, maxHitPoints);

	vga.active_buf->bar( x1, y1, x1+barWidth-1, y1+1, hitBarColor + HIT_BAR_BODY );

//	if( x1+barWidth <= x2 )
//		vga.blt_buf( x1+barWidth, y1, x2, y1+1, 0 );

	y1+=2;

	vga.active_buf->bar( x1, y1, x1+barWidth-1, y1, hitBarColor + HIT_BAR_DARK_BORDER );
	vga.active_buf->bar( x1+barWidth, y1, x1+barWidth, y1, V_BLACK );

//	if( x1+barWidth+1 <= x2 )
//		vga.blt_buf( x1+barWidth+1, y1, x2, y1, 0 );

	y1++;

	vga.active_buf->bar( x1+1, y1, x1+barWidth, y1, V_BLACK );

//	if( x1+barWidth+1 <= x2 )
//		vga.blt_buf( x1+barWidth+1, y1, x2, y1, 0 );
}
//----------- End of function disp_training_bar -----------//


//--------- Begin of function FirmCamp::disp_soldier_info ---------//
//
void FirmCamp::disp_soldier_info(int dispY1, int refreshFlag)
{
	disp_soldier_info_y1 = dispY1;

	if( selected_soldier_id==0 )		// display overseer info
	{
		disp_overseer_info(dispY1, refreshFlag);
		return;
	}

	if( selected_soldier_id > 0 && selected_soldier_id <= soldier_count )
	{
		int x=INFO_X1+20, y=dispY1;
		int x2;

		Soldier* soldierPtr = soldier_array + selected_soldier_id - 1;

		//------ if the unit is a living being -----//

		String str;

		if( soldierPtr->race_id )
		{
			if( soldierPtr->is_human() )
			{
				if( soldierPtr->hero_id )
					str  = hero_res[soldierPtr->hero_id]->name;
				else
					str  = race_res[soldierPtr->race_id]->get_name(soldierPtr->name_id);	// unit name
				str += " (";
				str += race_res[soldierPtr->race_id]->name;		// unit type name 
			}
			else
			{
				str  = monster_res.get_name(soldierPtr->name_id);		//monster name
				str += " (";
				str += monster_res[soldierPtr->monster_id()]->name;		// monster type name 
			}

			str += ")";
		}
		else
		{
			str = unit_res[soldierPtr->unit_id]->name;

			//------- if the unit is not a living being -----//

			// ###### begin Gilbert 24/3 #######//
			// if( unit_res[soldierPtr->unit_id]->unit_class == UNIT_CLASS_WEAPON )
			if( unit_res[soldierPtr->unit_id]->class_info.has_weapon_version
				&& soldierPtr->get_weapon_version() > 1 )
			// ###### end Gilbert 24/3 #######//
			{
				str += " ";
				str += m.roman_number( soldierPtr->get_weapon_version() );
			}
		}

		font_snds.put( x, y, str, 0, -1, 1 );

		//------------------------------------------------//

		// line spacing 24

		// ##### begin Gilbert 24/3 ##########//

		// ------- display loyalty ---------//

		UnitInfo *unitInfo = unit_res[soldierPtr->unit_id];

		if( unitInfo->class_info.has_loyalty && nation_recno )
		{
			if (soldierPtr->loyalty != soldierPtr->target_loyalty(firm_recno))
				info.disp_loyalty( x, y+12, INFO_X2-99 - font_snds.text_width(m.format(soldierPtr->loyalty, 4)) -
						font_snds.text_width(m.format(soldierPtr->target_loyalty(firm_recno), 4)) -
						font_snds.text_width("11"),
						soldierPtr->loyalty, soldierPtr->target_loyalty(firm_recno), nation_recno, refreshFlag, disp_combat_or_skill==4 );
			else
				info.disp_loyalty( x, y+12, INFO_X2-99 - font_snds.text_width(m.format(soldierPtr->loyalty, 4)),
						soldierPtr->loyalty, soldierPtr->target_loyalty(firm_recno), nation_recno, refreshFlag, disp_combat_or_skill==4 );
		}

		// ------- display combat ----------//

		if( unitInfo->class_info.has_combat_level )
		{
			// x2 = (disp_combat_or_skill==1?font_blu2:font_snds).put( x+110, y+12, "Combat" ) + 10;
			x2 = (disp_combat_or_skill==1?font_blu2:font_snds).put( x+110, y+12, text_unit.str_combat_level() ) + 10;
			font_snds.right_put( INFO_X2-10, y+12, m.format(soldierPtr->combat_level(),4) );
		}

		// ------- display leadership -------//

		if( unitInfo->class_info.has_skill_level )
		{
//			x2 = (disp_combat_or_skill==2?font_blu2:font_snds).put( x+110, y+26, "Leadership" ) + 10;
			x2 = (disp_combat_or_skill==2?font_blu2:font_snds).put( x+110, y+26, text_unit.str_leadership() ) + 10;
			font_snds.right_put( INFO_X2-10, y+26, m.format(soldierPtr->skill_level(),4) );
		}

		// ##### end Gilbert 24/3 ##########//

		// ----- display hit point ---------//

//		x2 = font_snds.put( x, y+26, "Hit Points" ) + 10;
		x2 = font_snds.put( x, y+26, text_unit.str_hit_points() ) + 10;
		str = m.format(soldierPtr->hit_points, 4);
		str += "/";
		str += m.format(soldierPtr->max_hit_points(), 4);
		font_snds.right_put( INFO_X2-100, y+26, str );
	}
}
//--------- End of function FirmCamp::disp_soldier_info ---------//


//--------- Begin of function FirmCamp::detect_soldier_info ---------//
//
void FirmCamp::detect_soldier_info()
{
	int dispY1 = disp_soldier_info_y1;

	if( selected_soldier_id==0 )		// display overseer info
	{
		detect_overseer_info();
		return;
	}

	int x=INFO_X1+20, y=dispY1;
	if( selected_soldier_id > 0 && selected_soldier_id <= soldier_count )
	{
		Soldier *soldierPtr = soldier_array + selected_soldier_id - 1;
		UnitInfo *unitInfo = unit_res[soldierPtr->unit_id];

		if( unitInfo->class_info.has_loyalty && nation_recno
			&& mouse.single_click( x, y+14, x+79 , y+27 ) )
		{
			// detect loyalty
			disp_combat_or_skill = 4;
		}
		else if( unitInfo->class_info.has_combat_level
			&& mouse.single_click( x+110, y+14, x+189, y+27 ) )
		{
			// detect combat level
			disp_combat_or_skill = 1;
		}
		else if( unitInfo->class_info.has_skill_level
			&& soldierPtr->skill_level() > 0
			&& mouse.single_click( x+110, y+28, x+189, y+41 ) )
		{
			// detect skill
			disp_combat_or_skill = 2;
		}
		else if( mouse.single_click( x-4, y-4, x+200, y+47 ) )
		{
			disp_combat_or_skill = 0;
		}
	}

	help.set_help( x, y+14, x+79, y+27, "LOYALTY" );
	help.set_help( x+110, y+14, x+189, y+27, "COMBAT" );
	help.set_help( x+110, y+28, x+189, y+41, "LEADER" );
//	help.set_help( x, y+28, x+79, y+41, "HITPOINT" );
}
//--------- End of function FirmCamp::detect_soldier_info ---------//


//--------- Begin of function FirmCamp::disp_overseer_info ---------//
//
void FirmCamp::disp_overseer_info(int dispY1, int refreshFlag)
{
	disp_soldier_info_y1 = dispY1;

	if( !overseer_recno )
		return;

	Unit* unitPtr = unit_array[overseer_recno];
	UnitInfo *unitInfo = unit_res[unitPtr->unit_id];

	int x=INFO_X1+20, y=dispY1;
	int x2;

	// --- display name_id, can spot spy ----- //

	String str;

	str  = unitPtr->unit_name();
	str += " (";

	if( unitPtr->is_human() )
		str += race_res[unitPtr->race_id]->name;
	else
		str += monster_res[unitPtr->monster_id()]->name;

	str += ")";

	font_snds.put( x, y, str, 0, INFO_X2-8, 1 );

	// line spacing 24

	// ------- display loyalty ---------//

	if( unitInfo->class_info.has_loyalty && unitPtr->rank_id != RANK_KING && unitPtr->nation_recno )
	{
		err_when( unitPtr->unit_id == UNIT_WAGON );

		if (unitPtr->loyalty != unitPtr->target_loyalty)
			info.disp_loyalty( x, y+12, INFO_X2-99 - font_snds.text_width(m.format(unitPtr->loyalty, 4)) -
						font_snds.text_width(m.format(unitPtr->target_loyalty, 4)) -
						font_snds.text_width("11"),
						unitPtr->loyalty, unitPtr->target_loyalty, nation_recno, refreshFlag, disp_combat_or_skill==4 );
		else
			info.disp_loyalty( x, y+12, INFO_X2-99 - font_snds.text_width(m.format(unitPtr->loyalty, 4)),
						unitPtr->loyalty, unitPtr->target_loyalty, nation_recno, refreshFlag, disp_combat_or_skill==4 );
	}

	// ------- display combat ----------//

#if(defined(CHINESE))
	y += 3;
#endif

	if( unitInfo->class_info.has_combat_level )
	{
		// x2 = (disp_combat_or_skill==1?font_blu2:font_snds).put( x+110, y+12, "Combat" ) + 10;
		//SXM
		x2 = (disp_combat_or_skill==1?font_blu2:font_snds).put( x+110, y+12, text_unit.str_combat_level() ) + 10;
		font_snds.right_put( INFO_X2-10, y+12, m.format(unitPtr->combat_level(),4) );
//		x2 = (disp_combat_or_skill==1?font_blu2:font_snds).put( x+110, y+15, text_unit.str_combat_level() ) + 10;
//		font_snds.right_put( INFO_X2-10, y+15, m.format(unitPtr->combat_level(),4) );
		//SXM
	}
		
		// ------- display leadership -------//

	if( unitInfo->class_info.has_skill_level && unitPtr->skill_level() > 0 )
	{
		// x2 = (disp_combat_or_skill==2?font_blu2:font_snds).put( x+110, y+26, "Leadership" ) + 10;
		//SXM
		x2 = (disp_combat_or_skill==2?font_blu2:font_snds).put( x+110, y+26, text_unit.str_leadership() ) + 10;
		font_snds.right_put( INFO_X2-10, y+26, m.format(unitPtr->skill_level(),4) );
//		x2 = (disp_combat_or_skill==2?font_blu2:font_snds).put( x+110, y+29, text_unit.str_leadership() ) + 10;
//		font_snds.right_put( INFO_X2-10, y+29, m.format(unitPtr->skill_level(),4) );
		//SXM
	}

	// ----- display hit point ---------//

//	x2 = font_snds.put( x, y+26, "Hit Points" ) + 10;
	//SXM
	x2 = font_snds.put( x, y+26, text_unit.str_hit_points() ) + 10;
//	x2 = font_snds.put( x, y+29, text_unit.str_hit_points() ) + 10;
	//SXM
	str = m.format((int)unitPtr->hit_points, 4);
	str += "/";
	str += m.format(unitPtr->max_hit_points(), 4);
	//SXM
	font_snds.right_put( INFO_X2-100, y+26, str );
//	font_snds.right_put( INFO_X2-100, y+29, str );
	//SXM
}
//----------- End of function FirmCamp::disp_overseer_info -----------//


//--------- Begin of function FirmCamp::detect_overseer_info ---------//
//
void FirmCamp::detect_overseer_info()
{
	int dispY1 = disp_soldier_info_y1;
	if( !overseer_recno )
		return;

	int x=INFO_X1+20, y=dispY1;

	Unit *unitPtr = unit_array[overseer_recno];
	UnitInfo *unitInfo = unit_res[unitPtr->unit_id];

	if( unitInfo->class_info.has_loyalty && nation_recno && unitPtr->rank_id != RANK_KING
		&& mouse.single_click( x, y+14, x+79 , y+27 ) )
	{
		// detect loyalty
		disp_combat_or_skill = 4;
	}
	else if( unitInfo->class_info.has_combat_level
		&& mouse.single_click( x+110, y+14, x+189, y+27 ) )
	{
		// detect combat level
		disp_combat_or_skill = 1;
	}
	else if( unitPtr->skill_level() > 0
		&& mouse.single_click( x+110, y+28, x+189, y+41 ) )
	{
		// detect skill
		disp_combat_or_skill = 2;
	}
	else if( mouse.single_click( x-4, y-4, x+200, y+47 ) )
	{
		disp_combat_or_skill = 0;
	}

	help.set_help( x, y+14, x+79, y+27, "LOYALTY" );
	help.set_help( x+110, y+14, x+189, y+27, "COMBAT" );
	help.set_help( x+110, y+28, x+189, y+41, "LEADER" );
//	help.set_help( x, y+28, x+79, y+41, "HITPOINT" );
}
//--------- End of function FirmCamp::detect_overseer_info ---------//


//--------- Begin of function FirmCamp::disp_train_menu ---------//
//
void FirmCamp::disp_train_menu(int refreshFlag)
{
	err_here();
}
//--------- End of function FirmCamp::disp_train_menu ---------//


//--------- Begin of function FirmCamp::detect_train_menu ---------//
//
void FirmCamp::detect_train_menu()
{
	err_here();
}
//--------- End of function FirmCamp::detect_train_menu ---------//


#ifdef DEBUG

//----------- Begin of static function disp_debug_info -----------//

static void disp_debug_info(FirmCamp* firmPtr, int refreshFlag)
{
	if( refreshFlag == INFO_REPAINT )
		vga.d3_panel_up( INFO_X1, INFO_Y2-40, INFO_X2, INFO_Y2 );

	int x=INFO_X1+3, y=INFO_Y2-37, x2=x+120;

	font_san.field( x, y   , "patrol unit count", x2, firmPtr->patrol_unit_count, 1, INFO_X2, refreshFlag);
	font_san.field( x, y+16, "coming unit count", x2, firmPtr->coming_unit_count, 1, INFO_X2, refreshFlag);

	font_san.put( x+180, y, firmPtr->firm_recno );
}
//----------- End of static function disp_debug_info -----------//

#endif


// ---- begin of function FirmCamp::explore_for_player --------//
//
int FirmCamp::explore_for_player()
{
	return Firm::explore_for_player()
		|| player_spy_count > 0;
}
// ---- end of function FirmCamp::explore_for_player --------//


// --------- begin of function FirmCamp::disp_edit_mode -------//
//
void FirmCamp::disp_edit_mode(int& refreshFlag, int& y, int dispBG)
{
	// display background here, set dispBG to 0

	if( dispBG )
	{
		vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("SCEN-ED") );
		dispBG = 0;
	}

	int x;
	int x1 = INFO_X1 + 12;
	int x2 = INFO_X2 - 10;

	char backupDisp = disp_combat_or_skill;
	disp_combat_or_skill = 0;	// normal
	disp_soldier_list( INFO_Y1, refreshFlag, 0 );
	disp_combat_or_skill = backupDisp;

	y = INFO_Y1+155;

	Firm::disp_edit_mode( refreshFlag, y, dispBG);

	// edit trainee

	edit_hp_enable = 0;
	edit_loyalty_enable = 0;
	edit_combat_enable = edit_max_combat_enable = 0;
	edit_skill_enable = edit_max_skill_enable = 0;
	edit_spy_enable = 0;
	clear_spy_enable = 0;
	add_spy_str[0] = add_spy_nation[0] = '\0';

	if( selected_soldier_id > 0 && selected_soldier_id <= soldier_count )
	{
		Soldier *soldierPtr = &soldier_array[selected_soldier_id-1];
		UnitInfo *unitInfo = unit_res[soldierPtr->unit_id];

		// display unit name name

		String str;
		if( soldierPtr->race_id )
		{
			if( soldierPtr->is_human() )
			{
				if( soldierPtr->hero_id )
					str  = hero_res[soldierPtr->hero_id]->name;
				else
					str  = race_res[soldierPtr->race_id]->get_name(soldierPtr->name_id);	// unit name
				str += " (";
				str += race_res[soldierPtr->race_id]->name;		// unit type name 
			}
			else
			{
				str  = monster_res.get_name(soldierPtr->name_id);		//monster name
				str += " (";
				str += monster_res[soldierPtr->monster_id()]->name;		// monster type name 
			}

			str += ")";
		}
		else
		{
			str = unit_res[soldierPtr->unit_id]->name;

			//------- if the unit is not a living being -----//

			if( unit_res[soldierPtr->unit_id]->class_info.has_weapon_version
				&& soldierPtr->get_weapon_version() > 1 )
			{
				str += " ";
				str += m.roman_number( soldierPtr->get_weapon_version() );
			}
		}
		font_whbl.put( x1, y, str, 0, x2, 1 );
		y += 15;

		// display hit point

		x = font_whbl.put( x1, y, text_unit.str_hit_points(), 0, x2 );	// "Hit Points"
		x = edit_hp_x2 = font_blue.put( (edit_hp_x1=x+20), (edit_hp_y1=y), (int)soldierPtr->hit_points, 0, x2 );
		edit_hp_y2 = edit_hp_y1+font_blue.max_font_height;
		edit_hp_enable = 1;
		x = font_whbl.put( x, y, " / ", 0, x2 );
		x = font_whbl.put( x, y, soldierPtr->max_hit_points(), 0, x2 );
		y += 15;

		// disp loyalty
		if( unitInfo->class_info.has_loyalty ) // && rank_id != RANK_KING )
		{
			x = font_whbl.put( x1, y, text_unit.str_loyalty(), 0, x2 );		// "Loyalty"
			x = edit_loyalty_x2 = font_blue.put( (edit_loyalty_x1=x+20), (edit_loyalty_y1=y), soldierPtr->loyalty, 0, x2 );
			edit_loyalty_y2 = edit_loyalty_y1 + font_blue.max_font_height;
			edit_loyalty_enable = 1;
			x = font_whbl.put( x, y, " -> ", 0, x2 );
			x = font_whbl.put( x, y, soldierPtr->target_loyalty(firm_recno), 0, x2 );
		}
		y += 15;

		// display combat actual(original)
		if( unitInfo->class_info.has_combat_level )
		{
			x = font_whbl.put( x1, y, text_unit.str_combat_level(), 0, x2 );//	"Combat Level"
			x = edit_combat_x2 = font_blue.put( (edit_combat_x1=x+20), (edit_combat_y1=y), soldierPtr->skill.actual_combat_level(NULL), 0, x2 );
			edit_combat_y2 = edit_combat_y1 + font_blue.max_font_height;
			edit_combat_enable = 1;
			if( soldierPtr->combat_level() != soldierPtr->skill.actual_combat_level(NULL) )
			{
				x = font_whbl.put( x, y, "(", 0, x2 );
				x = font_whbl.put( x, y, soldierPtr->combat_level(), 0, x2 );
				x = font_whbl.put( x, y, ")", 0, x2 );
			}
			if( sys.debug_session )		// maximum combat level is a higher secret
			{
				x = font_whbl.put( x, y, " / ", 0, x2 );
				x = edit_max_combat_x2 = font_blue.put( (edit_max_combat_x1=x), (edit_max_combat_y1=y), soldierPtr->skill.max_combat_level, 0, x2 );
				edit_max_combat_y2 = edit_max_combat_y1 + font_blue.max_font_height;
				edit_max_combat_enable = edit_combat_enable;
			}
		}
		y += 15;

		// display skill actual(original)
		if( unitInfo->class_info.has_skill_level )
		{
			x = font_whbl.put( x1, y, text_unit.str_leadership(), 0, x2 );	// "Leadership"
			x = edit_skill_x2 = font_blue.put( (edit_skill_x1=x+20), (edit_skill_y1=y), soldierPtr->skill.actual_skill_level(NULL), 0, x2 );
			edit_skill_y2 = edit_skill_y1 + font_blue.max_font_height - 1;
			edit_skill_enable = 1;
			if( soldierPtr->skill_level() != soldierPtr->skill.actual_skill_level(NULL) )
			{
				x = font_whbl.put( x, y, "(", 0, x2 );
				x = font_whbl.put( x, y, soldierPtr->skill_level(), 0, x2 );
				x = font_whbl.put( x, y, ")", 0, x2 );
			}
			if( sys.debug_session )		// maximum skill level is a higher secret
			{
				x = font_whbl.put( x, y, " / ", 0, x2 );
				x = edit_max_skill_x2 = font_blue.put( (edit_max_skill_x1=x), (edit_max_skill_y1=y), soldierPtr->skill.max_skill_level, 0, x2 );
				edit_max_skill_y2 = edit_max_skill_y1 + font_blue.max_font_height;
				edit_max_skill_enable = edit_skill_enable;
			}
		}
		y += 15;

		// display spy skill
		if( soldierPtr->spy_recno )
		{
			x = font_whbl.put( x1, y, text_unit.str_spy_skill(), 0, x2 );	// "Spy Skill"
			x += 20;
			// put true nation recno
			int trueNationRecno = spy_array[soldierPtr->spy_recno]->true_nation_recno;
			if( !nation_array.is_deleted(trueNationRecno) )
			{
				char trueNationChar[] = "1";
				trueNationChar[0] = FIRST_NATION_COLOR_CODE_IN_TEXT + nation_array[trueNationRecno]->color_scheme_id;
				clear_spy_enable = 1;
				x = clear_spy_x2 = font_whbl.put( (clear_spy_x1=x), (clear_spy_y1=y), trueNationChar, 0, x2 );
				clear_spy_y2 = clear_spy_y1 + NATION_COLOR_BAR_HEIGHT - 1;
				x += 10;
			}
			x = edit_spy_x2 = font_blue.put( edit_spy_x1=x, edit_spy_y1=y, spy_array[soldierPtr->spy_recno]->spy_skill, 0, x2 );
			edit_spy_y2 = edit_spy_y1 + font_blue.max_font_height;		
			edit_spy_enable = 1;
		}
		else if( soldierPtr->is_human() && soldierPtr->rank_id != RANK_KING )
		{
			// put nation colors for changing to spy
			x = font_whbl.put( x1, y, text_unit.str_spy_skill(), 0, x2 );	// "Spy Skill"
			x += 20;
			int nationCount = 0;
			for( char n = 1; n <= nation_array.size(); ++n )
			{
				if( !nation_array.is_deleted(n) )
				{
					add_spy_str[nationCount] = FIRST_NATION_COLOR_CODE_IN_TEXT + nation_array[n]->color_scheme_id;
					add_spy_nation[nationCount] = n;
					nationCount++;
				}
			}
			err_when( nationCount > MAX_NATION );
			add_spy_str[nationCount] = '\0';		// terminate string
			add_spy_nation[nationCount] = '\0';
			x = add_spy_x2 = font_whbl.put( (add_spy_x1=x), (add_spy_y1=y), add_spy_str, 0, x2 );
			add_spy_y2 = add_spy_y1 + NATION_COLOR_BAR_HEIGHT - 1;
		}
		y += 15;
	}
	else if( selected_soldier_id == 0 && !unit_array.is_truly_deleted(overseer_recno) )
	{
		Unit *overseerPtr = unit_array[overseer_recno];
		UnitInfo *unitInfo = unit_res[overseerPtr->unit_id];

		// display unit name name

		font_whbl.put( x1, y, overseerPtr->unit_name(1), 0, x2, 1 );
		y += 15;

		// display hit point

		x = font_whbl.put( x1, y, text_unit.str_hit_points(), 0, x2 );	// "Hit Points"
		x = edit_hp_x2 = font_blue.put( (edit_hp_x1=x+20), (edit_hp_y1=y), (int)overseerPtr->hit_points, 0, x2 );
		edit_hp_y2 = edit_hp_y1+font_blue.max_font_height;
		edit_hp_enable = 1;
		x = font_whbl.put( x, y, " / ", 0, x2 );
		x = font_whbl.put( x, y, overseerPtr->max_hit_points(), 0, x2 );
		y += 15;

		// disp loyalty
		if( unitInfo->class_info.has_loyalty ) // && rank_id != RANK_KING )
		{
			x = font_whbl.put( x1, y, text_unit.str_loyalty(), 0, x2 );		// "Loyalty"
			x = edit_loyalty_x2 = font_blue.put( (edit_loyalty_x1=x+20), (edit_loyalty_y1=y), overseerPtr->loyalty, 0, x2 );
			edit_loyalty_y2 = edit_loyalty_y1 + font_blue.max_font_height;
			edit_loyalty_enable = 1;
			x = font_whbl.put( x, y, " -> ", 0, x2 );
			x = font_whbl.put( x, y, overseerPtr->target_loyalty, 0, x2 );
		}
		y += 15;

		// display combat actual(original)
		if( unitInfo->class_info.has_combat_level )
		{
			x = font_whbl.put( x1, y, text_unit.str_combat_level(), 0, x2 );//	"Combat Level"
			x = edit_combat_x2 = font_blue.put( (edit_combat_x1=x+20), (edit_combat_y1=y), overseerPtr->skill.actual_combat_level(NULL), 0, x2 );
			edit_combat_y2 = edit_combat_y1 + font_blue.max_font_height;
			edit_combat_enable = 1;
			if( overseerPtr->combat_level() != overseerPtr->skill.actual_combat_level(NULL) )
			{
				x = font_whbl.put( x, y, "(", 0, x2 );
				x = font_whbl.put( x, y, overseerPtr->combat_level(), 0, x2 );
				x = font_whbl.put( x, y, ")", 0, x2 );
			}
			if( sys.debug_session )		// maximum combat level is a higher secret
			{
				x = font_whbl.put( x, y, " / ", 0, x2 );
				x = edit_max_combat_x2 = font_blue.put( (edit_max_combat_x1=x), (edit_max_combat_y1=y), overseerPtr->skill.max_combat_level, 0, x2 );
				edit_max_combat_y2 = edit_max_combat_y1 + font_blue.max_font_height;
				edit_max_combat_enable = edit_combat_enable;
			}
		}
		y += 15;

		// display skill actual(original)
		if( unitInfo->class_info.has_skill_level )
		{
			x = font_whbl.put( x1, y, text_unit.str_leadership(), 0, x2 );	// "Leadership"
			x = edit_skill_x2 = font_blue.put( (edit_skill_x1=x+20), (edit_skill_y1=y), overseerPtr->skill.actual_skill_level(NULL), 0, x2 );
			edit_skill_y2 = edit_skill_y1 + font_blue.max_font_height - 1;
			edit_skill_enable = 1;
			if( overseerPtr->skill_level() != overseerPtr->skill.actual_skill_level(NULL) )
			{
				x = font_whbl.put( x, y, "(", 0, x2 );
				x = font_whbl.put( x, y, overseerPtr->skill_level(), 0, x2 );
				x = font_whbl.put( x, y, ")", 0, x2 );
			}
			if( sys.debug_session )		// maximum skill level is a higher secret
			{
				x = font_whbl.put( x, y, " / ", 0, x2 );
				x = edit_max_skill_x2 = font_blue.put( (edit_max_skill_x1=x), (edit_max_skill_y1=y), overseerPtr->skill.max_skill_level, 0, x2 );
				edit_max_skill_y2 = edit_max_skill_y1 + font_blue.max_font_height;
				edit_max_skill_enable = edit_skill_enable;
			}
		}
		y += 15;

		// display spy skill
		if( overseerPtr->spy_recno )
		{
			x = font_whbl.put( x1, y, text_unit.str_spy_skill(), 0, x2 );	// "Spy Skill"
			x += 20;
			// put true nation recno
			int trueNationRecno = spy_array[overseerPtr->spy_recno]->true_nation_recno;
			if( !nation_array.is_deleted(trueNationRecno) )
			{
				char trueNationChar[] = "1";
				trueNationChar[0] = FIRST_NATION_COLOR_CODE_IN_TEXT + nation_array[trueNationRecno]->color_scheme_id;
				clear_spy_enable = 1;
				x = clear_spy_x2 = font_whbl.put( (clear_spy_x1=x), (clear_spy_y1=y), trueNationChar, 0, x2 );
				clear_spy_y2 = clear_spy_y1 + NATION_COLOR_BAR_HEIGHT - 1;
				x += 10;
			}
			x = edit_spy_x2 = font_blue.put( edit_spy_x1=x, edit_spy_y1=y, spy_array[overseerPtr->spy_recno]->spy_skill, 0, x2 );
			edit_spy_y2 = edit_spy_y1 + font_blue.max_font_height;		
			edit_spy_enable = 1;
		}
		else if( overseerPtr->is_human() && overseerPtr->rank_id != RANK_KING )
		{
			// put nation colors for changing to spy
			x = font_whbl.put( x1, y, text_unit.str_spy_skill(), 0, x2 );	// "Spy Skill"
			x += 20;
			int nationCount = 0;
			for( int n = 1; n <= nation_array.size(); ++n )
			{
				if( !nation_array.is_deleted(n) )
				{
					add_spy_str[nationCount] = FIRST_NATION_COLOR_CODE_IN_TEXT + nation_array[n]->color_scheme_id;
					add_spy_nation[nationCount] = n;
					nationCount++;
				}
			}
			err_when( nationCount > MAX_NATION );
			add_spy_str[nationCount] = '\0';		// terminate string
			add_spy_nation[nationCount] = '\0';		// terminate string
			x = add_spy_x2 = font_whbl.put( (add_spy_x1=x), (add_spy_y1=y), add_spy_str, 0, x2 );
			add_spy_y2 = add_spy_y1 + NATION_COLOR_BAR_HEIGHT - 1;
		}
		y += 15;
	}
	else
	{
		y += 15 * 6;
	}
}
// --------- end of function FirmCamp::disp_edit_mode -------//


// --------- begin of function FirmCamp::detect_edit_mode -------//
//
int FirmCamp::detect_edit_mode()
{
	int rc;
	if( (rc = Firm::detect_edit_mode()) )
		return rc;

	if( detect_soldier_list(0) )
		return 1;

	if( selected_soldier_id > 0 && selected_soldier_id <= soldier_count )
	{
		Soldier *soldierPtr = &soldier_array[selected_soldier_id-1];

		// edit soldier hit points

		if( edit_hp_enable )
		{
			if( mouse.any_click(edit_hp_x1, edit_hp_y1, edit_hp_x2, edit_hp_y2, LEFT_BUTTON) )
			{
				soldierPtr->hit_points += 10;
				if( soldierPtr->hit_points > soldierPtr->max_hit_points() )
					soldierPtr->hit_points = soldierPtr->max_hit_points();
				return 1;
			}
			if( mouse.any_click(edit_hp_x1, edit_hp_y1, edit_hp_x2, edit_hp_y2, RIGHT_BUTTON) )
			{
				soldierPtr->hit_points -= 10;
				if( soldierPtr->hit_points < 1 )
					soldierPtr->hit_points = 1;
				return 1;
			}
		}

		if( edit_loyalty_enable )
		{
			if( mouse.any_click(edit_loyalty_x1, edit_loyalty_y1, edit_loyalty_x2, edit_loyalty_y2, LEFT_BUTTON) )
			{
				soldierPtr->loyalty += 10;
				if( soldierPtr->loyalty > 100 )
					soldierPtr->loyalty = 100;
				return 1;
			}
			if( mouse.any_click(edit_loyalty_x1, edit_loyalty_y1, edit_loyalty_x2, edit_loyalty_y2, RIGHT_BUTTON) )
			{
				soldierPtr->loyalty -= 10;
				if( soldierPtr->loyalty < 0 )
					soldierPtr->loyalty = 0;
				return 1;
			}
		}

		if( edit_combat_enable )
		{
			if( mouse.any_click(edit_combat_x1, edit_combat_y1, edit_combat_x2, edit_combat_y2, LEFT_BUTTON) )
			{
				soldierPtr->skill.inc_combat_level(5.0f);
				// change hit points?
				return 1;
			}
			if( mouse.any_click(edit_combat_x1, edit_combat_y1, edit_combat_x2, edit_combat_y2, RIGHT_BUTTON)
				&& !soldierPtr->is_under_training() )		// not allow decrease when basic training
			{
				int newCombat = (int)soldierPtr->skill.actual_combat_level(NULL) - 5;
				if( newCombat < CITIZEN_COMBAT_LEVEL )
					newCombat = CITIZEN_COMBAT_LEVEL;
				soldierPtr->skill.set_combat_level(newCombat);
				return 1;
			}
		}

		if( edit_max_combat_enable )
		{
			if( mouse.any_click(edit_max_combat_x1, edit_max_combat_y1, edit_max_combat_x2, edit_max_combat_y2, LEFT_BUTTON) )
			{
				int newLevel = soldierPtr->skill.max_combat_level;
				newLevel += 20;
				if( newLevel > 900 )
					newLevel = 900;
				soldierPtr->skill.set_max_combat_level(newLevel);
				return 1;
			}
			if( mouse.any_click(edit_max_combat_x1, edit_max_combat_y1, edit_max_combat_x2, edit_max_combat_y2, RIGHT_BUTTON) )
			{
				int newLevel = soldierPtr->skill.max_combat_level;
				newLevel -= 20;
				if( newLevel < MAX_COMBAT_TRAIN )
					newLevel = MAX_COMBAT_TRAIN;
				soldierPtr->skill.set_max_combat_level(newLevel);
				return 1;
			}
		}

		if( edit_skill_enable )
		{
			if( mouse.any_click(edit_skill_x1, edit_skill_y1, edit_skill_x2, edit_skill_y2, LEFT_BUTTON) )
			{
				soldierPtr->skill.inc_skill_level(5.0f);
				return 1;
			}
			if( mouse.any_click(edit_skill_x1, edit_skill_y1, edit_skill_x2, edit_skill_y2, RIGHT_BUTTON)
				&& !soldierPtr->is_under_training() )
			{
				int newSkill = (int)soldierPtr->skill.actual_skill_level(NULL) - 5;
				if( newSkill < CITIZEN_SKILL_LEVEL )
					newSkill = CITIZEN_SKILL_LEVEL;
				soldierPtr->skill.set_skill_level(newSkill);
				return 1;
			}
		}

		if( edit_max_skill_enable )
		{
			if( mouse.any_click(edit_max_skill_x1, edit_max_skill_y1, edit_max_skill_x2, edit_max_skill_y2, LEFT_BUTTON) )
			{
				int newLevel = soldierPtr->skill.max_skill_level;
				newLevel += 10;
				if( newLevel > 200 )
					newLevel = 200;
				soldierPtr->skill.set_max_skill_level(newLevel);
				return 1;
			}
			if( mouse.any_click(edit_max_skill_x1, edit_max_skill_y1, edit_max_skill_x2, edit_max_skill_y2, RIGHT_BUTTON) )
			{
				int newLevel = soldierPtr->skill.max_skill_level;
				newLevel -= 20;
				if( newLevel < MAX_SKILL_TRAIN )
					newLevel = MAX_SKILL_TRAIN;
				soldierPtr->skill.set_max_skill_level(newLevel);
				return 1;
			}
		}

		if( edit_spy_enable && soldierPtr->spy_recno )
		{
			Spy *spyPtr = spy_array[soldierPtr->spy_recno];
			if( mouse.any_click(edit_spy_x1, edit_spy_y1, edit_spy_x2, edit_spy_y2, LEFT_BUTTON) )
			{
				if( (spyPtr->spy_skill += 5) > 100 )
				{
					spyPtr->spy_skill = 100;
					return 1;
				}
			}
			if( mouse.any_click(edit_spy_x1, edit_spy_y1, edit_spy_x2, edit_spy_y2, RIGHT_BUTTON) )
			{
				if( (spyPtr->spy_skill -= 5) < 10 )
				{
					spyPtr->spy_skill = 10;
					return 1;
				}
			}
		}

		// clear spy

		if( clear_spy_enable && soldierPtr->spy_recno )
		{
			if( mouse.single_click(clear_spy_x1, clear_spy_y1, clear_spy_x2, clear_spy_y2, RIGHT_BUTTON) )
			{
				spy_array.del_spy( soldierPtr->spy_recno );
				soldierPtr->spy_recno = 0;
			}
		}

		// add spy

		if( add_spy_nation[0] && !soldierPtr->spy_recno )		// each character represents one nation
		{
			int interval = (add_spy_x2 - add_spy_x1 + 1) / strlen(add_spy_str);
			int x1 = add_spy_x1;
			int x2 = add_spy_x1 + interval - 1;
			for( char n = 0; add_spy_nation[n]; ++n, (x1+=interval), (x2+=interval) )
			{
				if( !nation_array.is_deleted(add_spy_nation[n])
					&& mouse.single_click(x1, add_spy_y1, x2, add_spy_y2, LEFT_BUTTON) )
				{
					soldierPtr->spy_recno = spy_array.add_spy();
					Spy *newSpy = spy_array[soldierPtr->spy_recno];

					newSpy->spy_skill = CITIZEN_SKILL_LEVEL;
					newSpy->action_mode = SPY_IDLE;
					newSpy->spy_loyalty = 100;
					newSpy->true_nation_recno = add_spy_nation[n];
					newSpy->cloaked_nation_recno = nation_recno;		// current nation of the firm
					newSpy->race_id = soldierPtr->race_id;
					newSpy->name_id = soldierPtr->name_id;

					err_when( newSpy->race_id < 1 || newSpy->race_id > MAX_RACE );

					//-- Spy always registers its name twice as his name will be freed up in deinit(). Keep an additional right because when a spy is assigned to a town, the normal program will free up the name id., so we have to keep an additional copy
					if( !newSpy->name_id )		// if this soldier does not have a name, give him one now as a spy must reserve a name (see below on use_name_id() for reasons)
						newSpy->name_id = race_res[newSpy->race_id]->get_new_name_id();
					else
						race_res[newSpy->race_id]->use_name_id( newSpy->name_id );
					newSpy->unique_id = soldierPtr->unique_id;
					newSpy->set_place( SPY_FIRM, firm_recno );
					break;
				}
			}
		}
	}
	else if( selected_soldier_id == 0 && !unit_array.is_truly_deleted(overseer_recno) )
	{
		Unit *overseerPtr = unit_array[overseer_recno];

		if( edit_hp_enable )
		{
			if( mouse.any_click(edit_hp_x1, edit_hp_y1, edit_hp_x2, edit_hp_y2, LEFT_BUTTON) )
			{
				overseerPtr->hit_points += 10.0f;
				if( overseerPtr->hit_points > (float) overseerPtr->max_hit_points() )
					overseerPtr->hit_points = (float) overseerPtr->max_hit_points();
				return 1;
			}
			if( mouse.any_click(edit_hp_x1, edit_hp_y1, edit_hp_x2, edit_hp_y2, RIGHT_BUTTON) )
			{
				overseerPtr->hit_points -= 10.0f;
				if( overseerPtr->hit_points < 1.0f )
					overseerPtr->hit_points = 1.0f;
				return 1;
			}
		}

		if( edit_loyalty_enable )
		{
			if( mouse.any_click(edit_loyalty_x1, edit_loyalty_y1, edit_loyalty_x2, edit_loyalty_y2, LEFT_BUTTON) )
			{
				overseerPtr->loyalty += 10;
				if( overseerPtr->loyalty > 100 )
					overseerPtr->loyalty = 100;
				return 1;
			}
			if( mouse.any_click(edit_loyalty_x1, edit_loyalty_y1, edit_loyalty_x2, edit_loyalty_y2, RIGHT_BUTTON) )
			{
				overseerPtr->loyalty -= 10;
				if( overseerPtr->loyalty < 0 )
					overseerPtr->loyalty = 0;
				return 1;
			}
		}

		if( edit_combat_enable )
		{
			if( mouse.any_click(edit_combat_x1, edit_combat_y1, edit_combat_x2, edit_combat_y2, LEFT_BUTTON) )
			{
				int newCombat = (int)overseerPtr->skill.actual_combat_level(NULL) + 5;
				if( newCombat > overseerPtr->skill.max_combat_level )
					newCombat = overseerPtr->skill.max_combat_level;
				overseerPtr->set_combat_level(newCombat);
				return 1;
			}
			if( mouse.any_click(edit_combat_x1, edit_combat_y1, edit_combat_x2, edit_combat_y2, RIGHT_BUTTON) )
			{
				int newCombat = (int)overseerPtr->skill.actual_combat_level(NULL) - 5;
				if( newCombat < CITIZEN_COMBAT_LEVEL )
					newCombat = CITIZEN_COMBAT_LEVEL;
				overseerPtr->set_combat_level(newCombat);
				return 1;
			}
		}

		if( edit_max_combat_enable )
		{
			if( mouse.any_click(edit_max_combat_x1, edit_max_combat_y1, edit_max_combat_x2, edit_max_combat_y2, LEFT_BUTTON) )
			{
				int newLevel = overseerPtr->skill.max_combat_level;
				newLevel += 20;
				if( newLevel > 900 )
					newLevel = 900;
				overseerPtr->skill.set_max_combat_level(newLevel);
				return 1;
			}
			if( mouse.any_click(edit_max_combat_x1, edit_max_combat_y1, edit_max_combat_x2, edit_max_combat_y2, RIGHT_BUTTON) )
			{
				int newLevel = overseerPtr->skill.max_combat_level;
				newLevel -= 20;
				if( newLevel < MAX_COMBAT_TRAIN )
					newLevel = MAX_COMBAT_TRAIN;
				overseerPtr->skill.set_max_combat_level(newLevel);
				return 1;
			}
		}

		if( edit_skill_enable )
		{
			if( mouse.any_click(edit_skill_x1, edit_skill_y1, edit_skill_x2, edit_skill_y2, LEFT_BUTTON) )
			{
				overseerPtr->skill.inc_skill_level(5.0f);
				return 1;
			}
			if( mouse.any_click(edit_skill_x1, edit_skill_y1, edit_skill_x2, edit_skill_y2, RIGHT_BUTTON) )
			{
				int newSkill = (int)overseerPtr->skill.actual_skill_level(NULL) - 5;
				if( newSkill < CITIZEN_SKILL_LEVEL )
					newSkill = CITIZEN_SKILL_LEVEL;
				overseerPtr->skill.set_skill_level(newSkill);
				return 1;
			}
		}

		if( edit_max_skill_enable )
		{
			if( mouse.any_click(edit_max_skill_x1, edit_max_skill_y1, edit_max_skill_x2, edit_max_skill_y2, LEFT_BUTTON) )
			{
				int newLevel = overseerPtr->skill.max_skill_level;
				newLevel += 10;
				if( newLevel > 200 )
					newLevel = 200;
				overseerPtr->skill.set_max_skill_level(newLevel);
				return 1;
			}
			if( mouse.any_click(edit_max_skill_x1, edit_max_skill_y1, edit_max_skill_x2, edit_max_skill_y2, RIGHT_BUTTON) )
			{
				int newLevel = overseerPtr->skill.max_skill_level;
				newLevel -= 20;
				if( newLevel < MAX_SKILL_TRAIN )
					newLevel = MAX_SKILL_TRAIN;
				overseerPtr->skill.set_max_skill_level(newLevel);
				return 1;
			}
		}

		if( edit_spy_enable && overseerPtr->spy_recno )
		{
			Spy *spyPtr = spy_array[overseerPtr->spy_recno];
			if( mouse.any_click(edit_spy_x1, edit_spy_y1, edit_spy_x2, edit_spy_y2, LEFT_BUTTON) )
			{
				if( (spyPtr->spy_skill += 5) > 100 )
				{
					spyPtr->spy_skill = 100;
					return 1;
				}
			}
			if( mouse.any_click(edit_spy_x1, edit_spy_y1, edit_spy_x2, edit_spy_y2, RIGHT_BUTTON) )
			{
				if( (spyPtr->spy_skill -= 5) < 10 )
				{
					spyPtr->spy_skill = 10;
					return 1;
				}
			}
		}

		// clear spy

		if( clear_spy_enable && overseerPtr->spy_recno )
		{
			if( mouse.single_click(clear_spy_x1, clear_spy_y1, clear_spy_x2, clear_spy_y2, RIGHT_BUTTON) )
			{
				spy_array.del_spy( overseerPtr->spy_recno );
				overseerPtr->spy_recno = 0;
			}
		}

		// add spy

		if( add_spy_nation[0] && !overseerPtr->spy_recno )		// each character represents one nation
		{
			int interval = (add_spy_x2 - add_spy_x1 + 1) / strlen(add_spy_str);
			int x1 = add_spy_x1;
			int x2 = add_spy_x1 + interval - 1;
			for( char n = 0; add_spy_nation[n]; ++n, (x1+=interval), (x2+=interval) )
			{
				if( !nation_array.is_deleted(add_spy_nation[n])
					&& mouse.single_click(x1, add_spy_y1, x2, add_spy_y2, LEFT_BUTTON) )
				{
					overseerPtr->spy_recno = spy_array.add_spy();
					Spy *newSpy = spy_array[overseerPtr->spy_recno];

					newSpy->spy_skill = CITIZEN_SKILL_LEVEL;
					newSpy->action_mode = SPY_IDLE;
					newSpy->spy_loyalty = 100;
					newSpy->true_nation_recno = add_spy_nation[n];
					newSpy->cloaked_nation_recno = nation_recno;		// current nation of the firm
					newSpy->race_id = overseerPtr->race_id;
					newSpy->name_id = overseerPtr->name_id;

					err_when( newSpy->race_id < 1 || newSpy->race_id > MAX_RACE );

					//-- Spy always registers its name twice as his name will be freed up in deinit(). Keep an additional right because when a spy is assigned to a town, the normal program will free up the name id., so we have to keep an additional copy
					race_res[newSpy->race_id]->use_name_id( newSpy->name_id );
					newSpy->unique_id = overseerPtr->unique_id;
					newSpy->set_place( SPY_FIRM, firm_recno );
					break;
				}
			}
		}
	}

	return 0;
}
// --------- end of function FirmCamp::detect_edit_mode -------//
