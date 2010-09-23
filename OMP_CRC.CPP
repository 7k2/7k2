//Filename    : OMP_CRC.CPP
//Description : crc checking for multiplayer debugging
//Owner		  : Alex

#include <CRC.H>
#include <OBASEOBJ.H>
#include <OSPRITE.H>
#include <OUNITALL.H>
#include <OPLACE.H>
#include <OFIRMALL.H>
#include <OTOWN.H>
#include <ONATIONB.H>
#include <ONATIONA.H>
#include <OBULLET.H>
#include <OB_PROJ.H>
#include <OB_HOMIN.H>
#include <OB_FLAME.H>
#include <OB_PROG.H>
#include <OREBEL.H>
#include <OSPY.H>
#include <OTALKRES.H>
#include <OSITE.H>

#define RTRIM_ARRAY(a,s) { if(s<sizeof(a)/sizeof(*a)) memset(a+s,0,sizeof(a)-s*sizeof(*a)); }

static union
{
	char	base_obj[sizeof(BaseObj)];
	char	sprite[sizeof(Sprite)];
	char	unit_b[sizeof(UnitB)];
	char	unit[sizeof(Unit)];
	char	unit_god[sizeof(UnitGod)];
	char	unit_monster[sizeof(UnitMonster)];
	char	unit_exp_cart[sizeof(UnitExpCart)];
	//char	unit_marine[sizeof(UnitMarine)];
	char	unit_caravan[sizeof(UnitCaravan)];
	char	unit_wagon[sizeof(UnitWagon)];
	char	place[sizeof(Place)];
	char	firm[sizeof(Firm)];
	char	firm_base[sizeof(FirmBase)];
	char	firm_camp[sizeof(FirmCamp)];
	char	firm_fort[sizeof(FirmFort)];
	char	firm_lair[sizeof(FirmLair)];
	char	firm_work[sizeof(FirmWork)];
	char	firm_mine[sizeof(FirmMine)];
	char	firm_factory[sizeof(FirmFactory)];
	char	firm_research[sizeof(FirmResearch)];
	char	firm_war[sizeof(FirmWar)];
	char	firm_inn[sizeof(FirmInn)];
	char	firm_market[sizeof(FirmMarket)];
//	char	firm_harbor[sizeof(FirmHarbor)];
	char	firm_train[sizeof(FirmTrain)];
	char	firm_spy[sizeof(FirmSpy)];
	char	firm_special[sizeof(FirmSpecial)];
	char	firm_lishorr[sizeof(FirmLishorr)];
	char	firm_offensive2[sizeof(FirmOffensive2)];
	char	firm_monster_train[sizeof(FirmMonsterTrain)];
	char	firm_animal[sizeof(FirmAnimal)];
	char	firm_monster_alchemy[sizeof(FirmMonsterAlchemy)];
	char	firm_monster_fortress[sizeof(FirmMonsterFortress)];
	char	firm_incubator[sizeof(FirmIncubator)];
	char	firm_magic[sizeof(FirmMagic)];
	char	town[sizeof(Town)];
	char	nation[sizeof(NationBase)];
	char	bullet[sizeof(Bullet)];
	char	projectile[sizeof(Projectile)];
	char	bullet_homing[sizeof(BulletHoming)];
	char	bullet_program[sizeof(BulletProgram)];
	char	bullet_flame[sizeof(BulletFlame)];
	char	rebel[sizeof(Rebel)];
	char	spy[sizeof(Spy)];
	char	site[sizeof(Site)];
	char	talk_msg[sizeof(TalkMsg)];
} temp_obj;


//----------- Begin of function BaseObj::crc8 -----------//
UCHAR BaseObj::crc8()
{
	BaseObj &dummyBaseObj = *(BaseObj *)temp_obj.base_obj;
	memcpy(&dummyBaseObj, this, sizeof(BaseObj));

	dummyBaseObj.clear_ptr();
	*((char**) &dummyBaseObj) = NULL;

	UCHAR c = ::crc8((UCHAR*)&dummyBaseObj, sizeof(BaseObj));
	return c;
}
//----------- End of function BaseObj::crc8 -----------//


//----------- End of function BaseObj::clear_ptr -----------//
void BaseObj::clear_ptr()
{
}
//----------- End of function BaseObj::clear_ptr -----------//



//----------- Begin of function Sprite::crc8 -----------//
UCHAR Sprite::crc8()
{
	Sprite &dummySprite = *(Sprite *)temp_obj.sprite;
	memcpy(&dummySprite, this, sizeof(Sprite));

	dummySprite.clear_ptr();
	*((char**) &dummySprite) = NULL;

	UCHAR c = ::crc8((UCHAR*)&dummySprite, sizeof(Sprite));
	return c;
}
//----------- End of function Sprite::crc8 -----------//


//----------- End of function Sprite::clear_ptr -----------//
void Sprite::clear_ptr()
{
	BaseObj::clear_ptr();
	sprite_info = NULL;
}
//----------- End of function Sprite::clear_ptr -----------//


//----------- Begin of function UnitB::crc8 -----------//
UCHAR UnitB::crc8()
{
	UnitB &dummyUnitB = *(UnitB *)temp_obj.unit_b;
	memcpy(&dummyUnitB, this, sizeof(UnitB));

	dummyUnitB.clear_ptr();
	*((char**) &dummyUnitB) = NULL;

	UCHAR c = ::crc8((UCHAR*)&dummyUnitB, sizeof(UnitB));
	return c;
}
//----------- End of function UnitBase::crc8 -----------//


//----------- Begin of function UnitBase::clear_ptr -----------//
void UnitB::clear_ptr()
{
	Sprite::clear_ptr();

	selected_flag = 0;
	cur_path = NULL;
}
//----------- End of function UnitBase::clear_ptr -----------//



//----------- Begin of function Unit::crc8 -----------//
UCHAR Unit::crc8()
{
	Unit &dummyUnit = *(Unit *)temp_obj.unit;
	memcpy(&dummyUnit, this, sizeof(Unit));

	dummyUnit.clear_ptr();
	*((char**) &dummyUnit) = NULL;

	UCHAR c = ::crc8((UCHAR*)&dummyUnit, sizeof(Unit));
	return c;
}
//----------- End of function Unit::crc8 -----------//


//----------- Begin of function Unit::clear_ptr -----------//
void Unit::clear_ptr()
{
	UnitB::clear_ptr();
	attack_info_array = NULL;
   team_info = NULL;

	// ######## begin Gilbert 8/3 #######//
	if( max_power <= 0 )		// max_power is used for display money granted to the local player
		max_power = 0;
	// ######## end Gilbert 8/3 #######//

	// ####### begin Gilbert 16/3 ########//
	has_way_point = false;
	// ####### end Gilbert 16/3 ########//
}
//----------- End of function Unit::clear_ptr -----------//


//----------- Begin of function UnitGod::crc8 -----------//
UCHAR UnitGod::crc8()
{
	UnitGod &dummyUnitGod = *(UnitGod *)temp_obj.unit_god;
	memcpy(&dummyUnitGod, this, sizeof(UnitGod));

	dummyUnitGod.clear_ptr();
	*((char**) &dummyUnitGod) = NULL;

	UCHAR c = ::crc8((UCHAR*)&dummyUnitGod, sizeof(UnitGod));
	return c;
}
//----------- End of function UnitGod::crc8 -----------//


//----------- Begin of function UnitGod::clear_ptr -----------//
void UnitGod::clear_ptr()
{
	Unit::clear_ptr();
}
//----------- End of function UnitGod::clear_ptr -----------//


//----------- Begin of function UnitMonster::crc8 -----------//
UCHAR UnitMonster::crc8()
{
	UnitMonster &dummyUnitMonster = *(UnitMonster *)temp_obj.unit_monster;
	memcpy(&dummyUnitMonster, this, sizeof(UnitMonster));

	dummyUnitMonster.clear_ptr();
	*((char**) &dummyUnitMonster) = NULL;

	UCHAR c = ::crc8((UCHAR*)&dummyUnitMonster, sizeof(UnitMonster));
	return c;
}
//----------- End of function UnitMonster::crc8 -----------//


//----------- Begin of function UnitMonster::clear_ptr -----------//
void UnitMonster::clear_ptr()
{
	Unit::clear_ptr();
}
//----------- End of function UnitMonster::clear_ptr -----------//


//----------- Begin of function UnitExpCart::crc8 -----------//
UCHAR UnitExpCart::crc8()
{
	UnitExpCart &dummyUnitExpCart = *(UnitExpCart *)temp_obj.unit_exp_cart;
	memcpy(&dummyUnitExpCart, this, sizeof(UnitExpCart));

	dummyUnitExpCart.clear_ptr();
	*((char**) &dummyUnitExpCart) = NULL;

	UCHAR c = ::crc8((UCHAR*)&dummyUnitExpCart, sizeof(UnitExpCart));
	return c;
}
//----------- End of function UnitExpCart::crc8 -----------//


//----------- Begin of function UnitExpCart::clear_ptr -----------//
void UnitExpCart::clear_ptr()
{
	Unit::clear_ptr();
}
//----------- End of function UnitExpCart::clear_ptr -----------//


/*
//----------- End of function UnitMarine::crc8 -----------//
UCHAR UnitMarine::crc8()
{
	UnitMarine &dummyUnitMarine = *(UnitMarine *)temp_obj.unit_marine;
	memcpy(&dummyUnitMarine, this, sizeof(UnitMarine));

	dummyUnitMarine.clear_ptr();
	*((char**) &dummyUnitMarine) = NULL;

	UCHAR c = ::crc8((UCHAR*)&dummyUnitMarine, sizeof(UnitMarine));
	return c;
}
//----------- End of function UnitMarine::crc8 -----------//


//----------- End of function UnitMarine::clear_ptr -----------//
void UnitMarine::clear_ptr()
{
	memset(&splash, 0, sizeof(splash));
	
	//### begin alex 23/10 ###//
	selected_unit_id = 0;
	menu_mode = 0;
	//#### end alex 23/10 ####//

	// ###### patch begin Gilbert 21/1 ######//
	// must do this step before clear_ptr(), attack_info_array is reset there
	if( !attack_info_array )
		memset(&ship_attack_info, 0, sizeof(ship_attack_info));
	// ###### patch end Gilbert 21/1 ######//

	Unit::clear_ptr();

	RTRIM_ARRAY(unit_recno_array, unit_count);
	for( int i = 0; i < sizeof(stop_array)/sizeof(*stop_array); ++i)
	{
		if( !stop_array[i].firm_recno )
		{
			memset(&stop_array[i], 0, sizeof(*stop_array));
		}
	}
}
//----------- End of function UnitMarine::clear_ptr -----------//
*/

//----------- Begin of function UnitCaravan::crc8 -----------//
UCHAR UnitCaravan::crc8()
{
	UnitCaravan &dummyUnitCaravan = *(UnitCaravan *)temp_obj.unit_caravan;
	memcpy(&dummyUnitCaravan, this, sizeof(UnitCaravan));

	dummyUnitCaravan.clear_ptr();
	*((char**) &dummyUnitCaravan) = NULL;

	UCHAR c = ::crc8((UCHAR*)&dummyUnitCaravan, sizeof(UnitCaravan));
	return c;
}
//----------- End of function UnitCaravan::crc8 -----------//


//----------- Begin of function UnitCaravan::clear_ptr -----------//
void UnitCaravan::clear_ptr()
{
	Unit::clear_ptr();

	caravan_id = 0;	// caravan_id is no longer valid

	for( int i = 0; i < sizeof(stop_array)/sizeof(*stop_array); ++i)
	{
		if( !stop_array[i].firm_recno )
		{
			memset(&stop_array[i], 0, sizeof(*stop_array));
		}
	}
}
//----------- End of function UnitCaravan::clear_ptr -----------//


//----------- Begin of function UnitWagon::crc8 -----------//
UCHAR UnitWagon::crc8()
{
	UnitWagon &dummyUnitWagon = *(UnitWagon *)temp_obj.unit_wagon;
	memcpy(&dummyUnitWagon, this, sizeof(UnitWagon));

	dummyUnitWagon.clear_ptr();
	*((char**) &dummyUnitWagon) = NULL;

	UCHAR c = ::crc8((UCHAR*)&dummyUnitWagon, sizeof(UnitWagon));
	return c;
}
//----------- End of function UnitWagon::crc8 -----------//


//----------- Begin of function UnitWagon::clear_ptr -----------//
void UnitWagon::clear_ptr()
{
	Unit::clear_ptr();

	RTRIM_ARRAY(spy_recno, spy_count);
}
//----------- End of function UnitWagon::clear_ptr -----------//


//----------- Begin of function Place::crc8 -----------//
UCHAR Place::crc8()
{
	Place &dummyPlace = *(Place *)temp_obj.place;
	memcpy(&dummyPlace, this, sizeof(Place));

	dummyPlace.clear_ptr();
	*((char**) &dummyPlace) = NULL;

	UCHAR c = ::crc8((UCHAR*)&dummyPlace, sizeof(Place));
	return c;
}
//----------- End of function Place::crc8 -----------//


//----------- Begin of function Place::clear_ptr -----------//
void Place::clear_ptr()
{
	BaseObj::clear_ptr();
}
//----------- End of function Place::clear_ptr -----------//


//----------- Begin of function Firm::crc8 -----------//
UCHAR Firm::crc8()
{
	Firm &dummyFirm = *(Firm *)temp_obj.firm;
	memcpy(&dummyFirm, this, sizeof(Firm));

	dummyFirm.clear_ptr();
	*((char**) &dummyFirm) = NULL;

	UCHAR c = ::crc8((UCHAR*)&dummyFirm, sizeof(Firm));
	return c;
}
//----------- End of function Firm::crc8 -----------//


//----------- Begin of function Firm::clear_ptr -----------//
void Firm::clear_ptr()
{
	Place::clear_ptr();

//	worker_array = NULL;
//	selected_worker_id = 0;
//	player_spy_count = 0;

	// use RTRIM_ARRAY if possible
	memset( firm_cur_frame, 0, sizeof(firm_cur_frame));
	memset( firm_remain_frame_delay, 0, sizeof(firm_remain_frame_delay));

	// clear unused element in linked_firm_array, linked_firm_enable_array
	RTRIM_ARRAY(linked_firm_array, linked_firm_count);
	RTRIM_ARRAY(linked_firm_enable_array, linked_firm_count);

	// clear unused element in linked_town_array, linked_town_enable_array
	RTRIM_ARRAY(linked_town_array, linked_town_count);
	RTRIM_ARRAY(linked_town_enable_array, linked_town_count);

	reveal_info = 0;	// local player variable
}
//----------- End of function Firm::clear_ptr -----------//


//----------- Begin of function FirmBase::crc8 -----------//
UCHAR FirmBase::crc8()
{
	FirmBase &dummyFirmBase = *(FirmBase *)temp_obj.firm_base;
	memcpy(&dummyFirmBase, this, sizeof(FirmBase));

	dummyFirmBase.clear_ptr();
	*((char**) &dummyFirmBase) = NULL;

	UCHAR c = ::crc8((UCHAR*)&dummyFirmBase, sizeof(FirmBase));
	return c;
}
//----------- End of function FirmBase::crc8 -----------//


//----------- Begin of function FirmBase::clear_ptr -----------//
void FirmBase::clear_ptr()
{
	Firm::clear_ptr();

	// unimportant variable
	invoked_effect_id = 0;
	invoked_effect_para = 0;
	invoked_effect_last = 0;
}
//----------- End of function FirmBase::clear_ptr -----------//


//----------- Begin of function FirmCamp::crc8 -----------//
UCHAR FirmCamp::crc8()
{
	FirmCamp &dummyFirmCamp = *(FirmCamp *)temp_obj.firm_camp;
	memcpy(&dummyFirmCamp, this, sizeof(FirmCamp));

	dummyFirmCamp.clear_ptr();
	*((char**) &dummyFirmCamp) = NULL;

	UCHAR c = ::crc8((UCHAR*)&dummyFirmCamp, sizeof(FirmCamp));
	return c;
}
//----------- End of function FirmCamp::crc8 -----------//


//----------- Begin of function FirmCamp::clear_ptr -----------//
void FirmCamp::clear_ptr()
{
	Firm::clear_ptr();

	RTRIM_ARRAY(soldier_array, soldier_count);

	selected_soldier_id = 0;		// local player variable
	player_spy_count = 0;

	// clear unused element in patrol_unit_array
	RTRIM_ARRAY(patrol_unit_array, patrol_unit_count);
	
	// clear unused element in coming_unit_array
	RTRIM_ARRAY(coming_unit_array, coming_unit_count);
}
//----------- End of function FirmCamp::clear_ptr -----------//


//----------- Begin of function FirmFort::crc8 -----------//
UCHAR FirmFort::crc8()
{
	FirmFort &dummyFirmFort = *(FirmFort *)temp_obj.firm_fort;
	memcpy(&dummyFirmFort, this, sizeof(FirmFort));

	dummyFirmFort.clear_ptr();
	*((char**) &dummyFirmFort) = NULL;

	UCHAR c = ::crc8((UCHAR*)&dummyFirmFort, sizeof(FirmFort));
	return c;
}
//----------- End of function FirmFort::crc8 -----------//


//----------- Begin of function FirmFort::clear_ptr -----------//
void FirmFort::clear_ptr()
{
	FirmCamp::clear_ptr();

	RTRIM_ARRAY( last_archer_fire_frame, current_archer_count );
}
//----------- End of function FirmFort::clear_ptr -----------//


//----------- Begin of function FirmLair::crc8 -----------//
UCHAR FirmLair::crc8()
{
	FirmLair &dummyFirmLair = *(FirmLair *)temp_obj.firm_lair;
	memcpy(&dummyFirmLair, this, sizeof(FirmLair));

	dummyFirmLair.clear_ptr();
	*((char**) &dummyFirmLair) = NULL;

	UCHAR c = ::crc8((UCHAR*)&dummyFirmLair, sizeof(FirmLair));
	return c;
}
//----------- End of function FirmLair::crc8 -----------//


//----------- Begin of function FirmLair::clear_ptr -----------//
void FirmLair::clear_ptr()
{
	FirmCamp::clear_ptr();
}
//----------- End of function FirmLair::clear_ptr -----------//


//----------- Begin of function FirmWork::crc8 -----------//
UCHAR FirmWork::crc8()
{
	FirmWork &dummyFirmWork = *(FirmWork *)temp_obj.firm_work;
	memcpy(&dummyFirmWork, this, sizeof(FirmWork));

	dummyFirmWork.clear_ptr();
	*((char**) &dummyFirmWork) = NULL;

	UCHAR c = ::crc8((UCHAR*)&dummyFirmWork, sizeof(FirmWork));
	return c;
}
//----------- End of function FirmWork::crc8 -----------//


//----------- Begin of function FirmWork::clear_ptr -----------//
void FirmWork::clear_ptr()
{
	Firm::clear_ptr();

	RTRIM_ARRAY(worker_array, worker_count);

	selected_worker_id = 0;		// local player variable
}
//----------- End of function FirmWork::clear_ptr -----------//


//----------- Begin of function FirmMine::crc8 -----------//
UCHAR FirmMine::crc8()
{
	FirmMine &dummyFirmMine = *(FirmMine *)temp_obj.firm_mine;
	memcpy(&dummyFirmMine, this, sizeof(FirmMine));

	dummyFirmMine.clear_ptr();
	*((char**) &dummyFirmMine) = NULL;

	UCHAR c = ::crc8((UCHAR*)&dummyFirmMine, sizeof(FirmMine));
	return c;
}
//----------- End of function FirmMine::crc8 -----------//


//----------- Begin of function FirmMine::clear_ptr -----------//
void FirmMine::clear_ptr()
{
	FirmWork::clear_ptr();
}
//----------- End of function FirmMine::clear_ptr -----------//


//----------- Begin of function FirmFactory::crc8 -----------//
UCHAR FirmFactory::crc8()
{
	FirmFactory &dummyFirmFactory = *(FirmFactory *)temp_obj.firm_factory;
	memcpy(&dummyFirmFactory, this, sizeof(FirmFactory));

	dummyFirmFactory.clear_ptr();
	*((char**) &dummyFirmFactory) = NULL;

	UCHAR c = ::crc8((UCHAR*)&dummyFirmFactory, sizeof(FirmFactory));
	return c;
}
//----------- End of function FirmFactory::crc8 -----------//


//----------- Begin of function FirmFactory::clear_ptr -----------//
void FirmFactory::clear_ptr()
{
	FirmWork::clear_ptr();
}
//----------- End of function FirmFactory::clear_ptr -----------//


//----------- Begin of function FirmResearch::crc8 -----------//
UCHAR FirmResearch::crc8()
{
	FirmResearch &dummyFirmResearch = *(FirmResearch *)temp_obj.firm_research;
	memcpy(&dummyFirmResearch, this, sizeof(FirmResearch));

	dummyFirmResearch.clear_ptr();
	*((char**) &dummyFirmResearch) = NULL;

	UCHAR c = ::crc8((UCHAR*)&dummyFirmResearch, sizeof(FirmResearch));
	return c;
}
//----------- End of function FirmResearch::crc8 -----------//


//----------- Begin of function FirmResearch::clear_ptr -----------//
void FirmResearch::clear_ptr()
{
	FirmWork::clear_ptr();
}
//----------- End of function FirmResearch::clear_ptr -----------//


//----------- Begin of function FirmWar::crc8 -----------//
UCHAR FirmWar::crc8()
{
	FirmWar &dummyFirmWar = *(FirmWar *)temp_obj.firm_war;
	memcpy(&dummyFirmWar, this, sizeof(FirmWar));

	dummyFirmWar.clear_ptr();
	*((char**) &dummyFirmWar) = NULL;

	UCHAR c = ::crc8((UCHAR*)&dummyFirmWar, sizeof(FirmWar));
	return c;
}
//----------- End of function FirmWar::crc8 -----------//


//----------- Begin of function FirmWar::clear_ptr -----------//
void FirmWar::clear_ptr()
{
	FirmWork::clear_ptr();

	// if nothing building clear some variables
	if( !build_unit_id )
	{
		last_process_build_frame_no = 0;
		build_progress_days = (float)0.0;
	}

	// clear unused build_queue_array
	RTRIM_ARRAY(build_queue_array, build_queue_count);
}
//----------- End of function FirmWar::clear_ptr -----------//


//----------- Begin of function FirmInn::crc8 -----------//
UCHAR FirmInn::crc8()
{
	FirmInn &dummyFirmInn = *(FirmInn *)temp_obj.firm_inn;
	memcpy(&dummyFirmInn, this, sizeof(FirmInn));

	dummyFirmInn.clear_ptr();
	*((char**) &dummyFirmInn) = NULL;

	UCHAR c = ::crc8((UCHAR*)&dummyFirmInn, sizeof(FirmInn));
	return c;
}
//----------- End of function FirmInn::crc8 -----------//


//----------- Begin of function FirmInn::clear_ptr -----------//
void FirmInn::clear_ptr()
{
	Firm::clear_ptr();

	// selected_unit_id = 0;		// local player variable

	// clear unused element in inn_unit_array
	RTRIM_ARRAY(inn_unit_array, inn_unit_count);
}
//----------- End of function FirmInn::clear_ptr -----------//


//----------- Begin of function FirmMarket::crc8 -----------//
UCHAR FirmMarket::crc8()
{
	FirmMarket &dummyFirmMarket = *(FirmMarket *)temp_obj.firm_market;
	memcpy(&dummyFirmMarket, this, sizeof(FirmMarket));

	dummyFirmMarket.clear_ptr();
	*((char**) &dummyFirmMarket) = NULL;

	UCHAR c = ::crc8((UCHAR*)&dummyFirmMarket, sizeof(FirmMarket));
	return c;
}
//----------- End of function FirmMarket::crc8 -----------//


//----------- Begin of function FirmMarket::clear_ptr -----------//
void FirmMarket::clear_ptr()
{
	int i;
//	for(i=0; i<MAX_RAW; ++i)
//		market_raw_array[i] = NULL;

//	for(i=0; i<MAX_PRODUCT; ++i)
//		market_product_array[i] = NULL;

	// clear unused element in defense_array
	for( i = 0; i < sizeof(market_goods_array)/sizeof(*market_goods_array); ++i )
	{
		if( !market_goods_array[i].raw_id && !market_goods_array[i].product_raw_id )
		{
			memset(&market_goods_array[i], 0, sizeof(*market_goods_array));
		}
	}

	Firm::clear_ptr();
}
//----------- End of function FirmMarket::clear_ptr -----------//



/*
//----------- End of function FirmHarbor::crc8 -----------//
UCHAR FirmHarbor::crc8()
{
	FirmHarbor &dummyFirmHarbor = *(FirmHarbor *)temp_obj.firm_harbor;
	memcpy(&dummyFirmHarbor, this, sizeof(FirmHarbor));

	dummyFirmHarbor.clear_ptr();
	*((char**) &dummyFirmHarbor) = NULL;

	UCHAR c = ::crc8((UCHAR*)&dummyFirmHarbor, sizeof(FirmHarbor));
	return c;
}
//----------- End of function FirmHarbor::crc8 -----------//


//----------- Begin of function FirmHarbor::clear_ptr -----------//
void FirmHarbor::clear_ptr()
{
	Firm::clear_ptr();

	if(!build_unit_id)
		start_build_frame_no = 0;

	RTRIM_ARRAY(ship_recno_array, ship_count);
	RTRIM_ARRAY(build_queue_array, build_queue_count);
	RTRIM_ARRAY(linked_mine_array, linked_mine_num);
	RTRIM_ARRAY(linked_factory_array, linked_factory_num);
	RTRIM_ARRAY(linked_market_array, linked_market_num);
}
//----------- End of function FirmHarbor::clear_ptr -----------//
*/

//----------- End of function FirmTrain::crc8 -----------//
UCHAR FirmTrain::crc8()
{
	FirmTrain &dummyFirmTrain = *(FirmTrain *)temp_obj.firm_train;
	memcpy(&dummyFirmTrain, this, sizeof(FirmTrain));

	dummyFirmTrain.clear_ptr();
	*((char**) &dummyFirmTrain) = NULL;

	UCHAR c = ::crc8((UCHAR*)&dummyFirmTrain, sizeof(FirmTrain));
	return c;
}
//----------- End of function FirmTrain::crc8 -----------//


//----------- End of function FirmTrain::clear_ptr -----------//
void FirmTrain::clear_ptr()
{
	Firm::clear_ptr();

	selected_trainee_id = 0;	// local player variable
	player_spy_count = 0;		// local player variable

	RTRIM_ARRAY( trainee_array, trainee_count );
}
//----------- End of function FirmTrain::clear_ptr -----------//


//----------- Begin of function FirmSpy::crc8 -----------//
UCHAR FirmSpy::crc8()
{
	FirmSpy &dummyFirmSpy = *(FirmSpy *)temp_obj.firm_spy;
	memcpy(&dummyFirmSpy, this, sizeof(FirmSpy));

	dummyFirmSpy.clear_ptr();
	*((char**) &dummyFirmSpy) = NULL;

	UCHAR c = ::crc8((UCHAR*)&dummyFirmSpy, sizeof(FirmSpy));
	return c;
}
//----------- End of function FirmSpy::crc8 -----------//


//----------- Begin of function FirmSpy::clear_ptr -----------//
void FirmSpy::clear_ptr()
{
	FirmTrain::clear_ptr();
}
//----------- End of function FirmSpy::clear_ptr -----------//


//----------- Begin of function FirmSpecial::crc8 -----------//
UCHAR FirmSpecial::crc8()
{
	FirmSpecial &dummyFirmSpecial = *(FirmSpecial *)temp_obj.firm_special;
	memcpy(&dummyFirmSpecial, this, sizeof(FirmSpecial));

	dummyFirmSpecial.clear_ptr();
	*((char**) &dummyFirmSpecial) = NULL;

	UCHAR c = ::crc8((UCHAR*)&dummyFirmSpecial, sizeof(FirmSpecial));
	return c;
}
//----------- End of function FirmSpecial::crc8 -----------//


//----------- Begin of function FirmSpecial::clear_ptr -----------//
void FirmSpecial::clear_ptr()
{
	FirmTrain::clear_ptr();
}
//----------- End of function FirmSpecial::clear_ptr -----------//


//----------- Begin of function FirmLishorr::crc8 -----------//
UCHAR FirmLishorr::crc8()
{
	FirmLishorr &dummyFirmLishorr = *(FirmLishorr *)temp_obj.firm_lishorr;
	memcpy(&dummyFirmLishorr, this, sizeof(FirmLishorr));

	dummyFirmLishorr.clear_ptr();
	*((char**) &dummyFirmLishorr) = NULL;

	UCHAR c = ::crc8((UCHAR*)&dummyFirmLishorr, sizeof(FirmLishorr));
	return c;
}
//----------- End of function FirmLishorr::crc8 -----------//


//----------- Begin of function FirmLishorr::clear_ptr -----------//
void FirmLishorr::clear_ptr()
{
	Firm::clear_ptr();
}
//----------- End of function FirmLishorr::clear_ptr -----------//

//----------- Begin of function FirmOffensive2::crc8 -----------//
UCHAR FirmOffensive2::crc8()
{
	FirmOffensive2 &dummyFirmOffensive2 = *(FirmOffensive2 *)temp_obj.firm_offensive2;
	memcpy(&dummyFirmOffensive2, this, sizeof(FirmOffensive2));

	dummyFirmOffensive2.clear_ptr();
	*((char**) &dummyFirmOffensive2) = NULL;

	UCHAR c = ::crc8((UCHAR*)&dummyFirmOffensive2, sizeof(FirmOffensive2));
	return c;
}
//----------- End of function FirmOffensive2::crc8 -----------//


//----------- Begin of function FirmOffensive2::clear_ptr -----------//
void FirmOffensive2::clear_ptr()
{
	Firm::clear_ptr();
}
//----------- End of function FirmOffensive2::clear_ptr -----------//

//----------- Begin of function FirmMonsterTrain::crc8 -----------//
UCHAR FirmMonsterTrain::crc8()
{
	FirmMonsterTrain &dummyFirmMonsterTrain = *(FirmMonsterTrain *)temp_obj.firm_monster_train;
	memcpy(&dummyFirmMonsterTrain, this, sizeof(FirmMonsterTrain));

	dummyFirmMonsterTrain.clear_ptr();
	*((char**) &dummyFirmMonsterTrain) = NULL;

	UCHAR c = ::crc8((UCHAR*)&dummyFirmMonsterTrain, sizeof(FirmMonsterTrain));
	return c;
}
//----------- End of function FirmMonsterTrain::crc8 -----------//


//----------- Begin of function FirmMonsterTrain::clear_ptr -----------//
void FirmMonsterTrain::clear_ptr()
{
	Firm::clear_ptr();

	RTRIM_ARRAY( trainee_array, trainee_count );
	selected_trainee_id = 0;
}
//----------- End of function FirmMonsterTrain::clear_ptr -----------//


//----------- Begin of function FirmAnimal::crc8 -----------//
UCHAR FirmAnimal::crc8()
{
	FirmAnimal &dummyFirmAnimal = *(FirmAnimal *)temp_obj.firm_animal;
	memcpy(&dummyFirmAnimal, this, sizeof(FirmAnimal));

	dummyFirmAnimal.clear_ptr();
	*((char**) &dummyFirmAnimal) = NULL;

	UCHAR c = ::crc8((UCHAR*)&dummyFirmAnimal, sizeof(FirmAnimal));
	return c;
}
//----------- End of function FirmAnimal::crc8 -----------//


//----------- Begin of function FirmAnimal::clear_ptr -----------//
void FirmAnimal::clear_ptr()
{
	Firm::clear_ptr();

	RTRIM_ARRAY( animal_unit_array, animal_unit_count );
	// selected_unit_id = 0;		// don't reset static !
}
//----------- End of function FirmAnimal::clear_ptr -----------//


//----------- Begin of function FirmMonsterAlchemy::crc8 -----------//
UCHAR FirmMonsterAlchemy::crc8()
{
	FirmMonsterAlchemy &dummyFirmMonsterAlchemy = *(FirmMonsterAlchemy *)temp_obj.firm_monster_alchemy;
	memcpy(&dummyFirmMonsterAlchemy, this, sizeof(FirmMonsterAlchemy));

	dummyFirmMonsterAlchemy.clear_ptr();
	*((char**) &dummyFirmMonsterAlchemy) = NULL;

	UCHAR c = ::crc8((UCHAR*)&dummyFirmMonsterAlchemy, sizeof(FirmMonsterAlchemy));
	return c;
}
//----------- End of function FirmMonsterAlchemy::crc8 -----------//


//----------- Begin of function FirmMonsterAlchemy::clear_ptr -----------//
void FirmMonsterAlchemy::clear_ptr()
{
	Firm::clear_ptr();
}
//----------- End of function FirmMonsterAlchemy::clear_ptr -----------//


//----------- Begin of function FirmMonsterFortress::crc8 -----------//
UCHAR FirmMonsterFortress::crc8()
{
	FirmMonsterFortress &dummyFirmMonsterFortress = *(FirmMonsterFortress *)temp_obj.firm_monster_fortress;
	memcpy(&dummyFirmMonsterFortress, this, sizeof(FirmMonsterFortress));

	dummyFirmMonsterFortress.clear_ptr();
	*((char**) &dummyFirmMonsterFortress) = NULL;

	UCHAR c = ::crc8((UCHAR*)&dummyFirmMonsterFortress, sizeof(FirmMonsterFortress));
	return c;
}
//----------- End of function FirmMonsterFortress::crc8 -----------//


//----------- Begin of function FirmMonsterFortress::clear_ptr -----------//
void FirmMonsterFortress::clear_ptr()
{
	Firm::clear_ptr();

	RTRIM_ARRAY( extra_builder_recno, extra_builder_count );
	RTRIM_ARRAY( archer_unit_recno, archer_count );

	selected_archer_id = 0;		// local player variable

	RTRIM_ARRAY( target_base_obj_recno, target_count );
}
//----------- End of function FirmMonsterFortress::clear_ptr -----------//


//----------- Begin of function FirmIncubator::crc8 -----------//
UCHAR FirmIncubator::crc8()
{
	FirmIncubator &dummyFirmIncubator = *(FirmIncubator *)temp_obj.firm_incubator;
	memcpy(&dummyFirmIncubator, this, sizeof(FirmIncubator));

	dummyFirmIncubator.clear_ptr();
	*((char**) &dummyFirmIncubator) = NULL;

	UCHAR c = ::crc8((UCHAR*)&dummyFirmIncubator, sizeof(FirmIncubator));
	return c;
}
//----------- End of function FirmIncubator::crc8 -----------//


//----------- Begin of function FirmIncubator::clear_ptr -----------//
void FirmIncubator::clear_ptr()
{
	Firm::clear_ptr();

	// if nothing building clear some variables
	if( !build_unit_id )
	{
		last_process_build_frame_no = 0;
		build_progress_days = (float)0.0;
	}

	// clear unused build_queue_array
	RTRIM_ARRAY(build_queue_array, build_queue_count);
}
//----------- End of function FirmIncubator::clear_ptr -----------//


//----------- Begin of function FirmMagic::crc8 -----------//
UCHAR FirmMagic::crc8()
{
	FirmMagic &dummyFirmMagic = *(FirmMagic *)temp_obj.firm_magic;
	memcpy(&dummyFirmMagic, this, sizeof(FirmMagic));

	dummyFirmMagic.clear_ptr();
	*((char**) &dummyFirmMagic) = NULL;

	UCHAR c = ::crc8((UCHAR*)&dummyFirmMagic, sizeof(FirmMagic));
	return c;
}
//----------- End of function FirmMagic::crc8 -----------//


//----------- Begin of function FirmMagic::clear_ptr -----------//
void FirmMagic::clear_ptr()
{
	Firm::clear_ptr();

	// teleport_unit_recno = 0;	// don't change static variable !
}
//----------- End of function FirmMagic::clear_ptr -----------//


//----------- End of function Town::crc8 -----------//
UCHAR Town::crc8()
{
	Town &dummyTown = *(Town *)temp_obj.town;
	memcpy(&dummyTown, this, sizeof(Town));

	dummyTown.clear_ptr();
	*((char**) &dummyTown) = NULL;

	UCHAR c = ::crc8((UCHAR*)&dummyTown, sizeof(Town));
	return c;
}
//----------- End of function Town::crc8 -----------//


//----------- End of function Town::clear_ptr -----------//
void Town::clear_ptr()
{
//	memset( town_name, 0, sizeof(town_name) );

	int layoutCount = town_res.get_layout(layout_id)->slot_count;
	RTRIM_ARRAY(slot_object_id_array, layoutCount);

	RTRIM_ARRAY(linked_firm_array, linked_firm_count);
	RTRIM_ARRAY(linked_firm_enable_array, linked_firm_count);

	RTRIM_ARRAY(linked_town_array, linked_town_count);
	RTRIM_ARRAY(linked_town_enable_array, linked_town_count);

	RTRIM_ARRAY(target_base_obj_recno, target_count );
}
//----------- End of function Town::clear_ptr -----------//


//----------- Begin of function NationBase::crc8 -----------//
UCHAR NationBase::crc8()
{
	NationBase &dummyNationBase = *(NationBase *)temp_obj.nation;
	memcpy(&dummyNationBase, this, sizeof(NationBase));

	dummyNationBase.clear_ptr();
	*((char**) &dummyNationBase) = NULL;

	UCHAR c = ::crc8((UCHAR*)&dummyNationBase, sizeof(NationBase));
	return c;
}
//----------- End of function NationBase::crc8 -----------//


//----------- End of function NationBase::clear_ptr -----------//
void NationBase::clear_ptr()
{
	if( nation_type != NATION_AI )
		nation_type = 0;		// local setting
	campaign_nation_recno = 0;
	memset(nation_name_str, 0, sizeof(nation_name_str) );    // garbage may exist after the '\0'
	next_frame_ready = 0;
	is_allied_with_player = 0;
	increased_cash = 0.0f;		// displaying effect, no need to check sync

	// ignore contact_msg_flag in relation_array
	for(short nationRecno=1; nationRecno <= MAX_NATION; ++nationRecno)
	{
		if( !nation_array.is_deleted(nationRecno) )
		{
			get_relation(nationRecno)->contact_msg_flag = 0;
		}
		else
		{
			memset( &relation_array[nationRecno-1], 0, sizeof(NationRelation) );
			relation_status_array[nationRecno-1] = 0;
			relation_passable_array[nationRecno-1] = 0;
			relation_should_attack_array[nationRecno-1] = 0;
		}
	}
}
//----------- End of function NationBase::clear_ptr -----------//


//----------- Begin of function Bullet::crc8 -----------//
UCHAR Bullet::crc8()
{
	Bullet &dummyBullet = *(Bullet *)temp_obj.bullet;
	memcpy(&dummyBullet, this, sizeof(Bullet));

	dummyBullet.clear_ptr();
	*((char**) &dummyBullet) = NULL;

	UCHAR c = ::crc8((UCHAR*)&dummyBullet, sizeof(Bullet));
	return c;
}
//----------- End of function Bullet::crc8 -----------//


//----------- End of function Bullet::clear_ptr -----------//
void Bullet::clear_ptr()
{
	Sprite::clear_ptr();
}
//----------- End of function Bullet::clear_ptr -----------//


//----------- End of function Projectile::crc8 -----------//
UCHAR Projectile::crc8()
{
	Projectile &dummyProjectile = *(Projectile *)temp_obj.projectile;
	memcpy(&dummyProjectile, this, sizeof(Projectile));

	dummyProjectile.clear_ptr();
	*((char**) &dummyProjectile) = NULL;

	UCHAR c = ::crc8((UCHAR*)&dummyProjectile, sizeof(Projectile));
	return c;
}
//----------- End of function Projectile::crc8 -----------//


//----------- End of function Projectile::clear_ptr -----------//
void Projectile::clear_ptr()
{
	memset(&act_bullet, 0, sizeof(act_bullet));
	memset(&bullet_shadow, 0, sizeof(bullet_shadow));
	
	Bullet::clear_ptr();
}
//----------- End of function Projectile::clear_ptr -----------//


//----------- Begin of function BulletHoming::crc8 -----------//
UCHAR BulletHoming::crc8()
{
	BulletHoming &dummyBulletHoming = *(BulletHoming *)temp_obj.bullet_homing;
	memcpy(&dummyBulletHoming, this, sizeof(BulletHoming));

	dummyBulletHoming.clear_ptr();
	*((char**) &dummyBulletHoming) = NULL;

	UCHAR c = ::crc8((UCHAR*)&dummyBulletHoming, sizeof(BulletHoming));
	return c;
}
//----------- End of function BulletHoming::crc8 -----------//


//----------- Begin of function BulletHoming::clear_ptr -----------//
void BulletHoming::clear_ptr()
{
	Bullet::clear_ptr();
}
//----------- End of function BulletHoming::clear_ptr -----------//


//----------- Begin of function BulletProgram::crc8 -----------//
UCHAR BulletProgram::crc8()
{
	BulletProgram &dummyBulletProgram = *(BulletProgram *)temp_obj.bullet_program;
	memcpy(&dummyBulletProgram, this, sizeof(BulletProgram));

	dummyBulletProgram.clear_ptr();
	*((char**) &dummyBulletProgram) = NULL;

	UCHAR c = ::crc8((UCHAR*)&dummyBulletProgram, sizeof(BulletProgram));
	return c;
}
//----------- End of function BulletProgram::crc8 -----------//


//----------- Begin of function BulletProgram::clear_ptr -----------//
void BulletProgram::clear_ptr()
{
	Bullet::clear_ptr();
}
//----------- End of function BulletProgram::clear_ptr -----------//


//----------- End of function BulletFlame::crc8 -----------//
UCHAR BulletFlame::crc8()
{
	BulletFlame &dummyBulletFlame = *(BulletFlame *)temp_obj.bullet_flame;
	memcpy(&dummyBulletFlame, this, sizeof(BulletFlame));

	dummyBulletFlame.clear_ptr();
	*((char**) &dummyBulletFlame) = NULL;

	UCHAR c = ::crc8((UCHAR*)&dummyBulletFlame, sizeof(BulletFlame));
	return c;
}
//----------- End of function BulletFlame::crc8 -----------//


//----------- Begin of function BulletFlame::clear_ptr -----------//
void BulletFlame::clear_ptr()
{
	Bullet::clear_ptr();
}
//----------- End of function BulletFlame::clear_ptr -----------//


//----------- Begin of function Rebel::crc8 -----------//
UCHAR Rebel::crc8()
{
	Rebel &dummyRebel = *(Rebel *)temp_obj.rebel;
	memcpy(&dummyRebel, this, sizeof(Rebel));

	dummyRebel.clear_ptr();

	UCHAR c = ::crc8((UCHAR *)&dummyRebel, sizeof(Rebel));
	return c;
}
//----------- End of function Rebel::crc8 -----------//


//----------- Begin of function Rebel::clear_ptr -----------//
void Rebel::clear_ptr()
{
}
//----------- End of function Rebel::clear_ptr -----------//


//----------- Begin of function Spy::crc8 -----------//
UCHAR Spy::crc8()
{
	Spy &dummySpy = *(Spy *)temp_obj.spy;
	memcpy(&dummySpy, this, sizeof(Spy));

	dummySpy.clear_ptr();

	UCHAR c = ::crc8((UCHAR *)&dummySpy, sizeof(Spy));
	return c;
}
//----------- End of function Spy::crc8 -----------//


//----------- Begin of function Spy::clear_ptr -----------//
void Spy::clear_ptr()
{
}
//----------- End of function Spy::clear_ptr -----------//


//----------- Begin of function Site::crc8 -----------//
UCHAR Site::crc8()
{
	Site &dummySite = *(Site *)temp_obj.site;
	memcpy(&dummySite, this, sizeof(Site));

	dummySite.clear_ptr();

	if( (char *)&site_recno > (char *)this )		// clear virtual pointer for the future
		*((char**) &dummySite) = NULL;

	UCHAR c = ::crc8((UCHAR *)&dummySite, sizeof(Site));
	return c;
}
//----------- End of function Site::crc8 -----------//


//----------- Begin of function Site::clear_ptr -----------//
void Site::clear_ptr()
{
}
//----------- End of function Site::clear_ptr -----------//


//----------- Begin of function TalkMsg::crc8 -----------//
UCHAR TalkMsg::crc8()
{
	TalkMsg &dummyTalkMsg = *(TalkMsg *)temp_obj.talk_msg;
	memcpy(&dummyTalkMsg, this, sizeof(TalkMsg));

	dummyTalkMsg.clear_ptr();
	// *((char**) &dummyTalkMsg) = NULL;

	UCHAR c = ::crc8((UCHAR*)&dummyTalkMsg, sizeof(TalkMsg));
	return c;
}
//----------- End of function TalkMsg::crc8 -----------//

//----------- Begin of function TalkMsg::clear_ptr -----------//
void TalkMsg::clear_ptr()
{
}
//----------- End of function TalkMsg::clear_ptr -----------//
