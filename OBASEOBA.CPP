//Filename   : OBASEOBJ.CPP
//Description: BaseObj Array

#include <OUNIT.H>
#include <OFIRM.H>
#include <OTOWN.H>
#include <OBASEOBJ.H>

//--------- Begin of function BaseObjArray::BaseObjArray ---------//
//
BaseObjArray::BaseObjArray() : DynArrayB( sizeof(BaseObj*), 500, DEFAULT_REUSE_INTERVAL_DAYS )
{
}
//----------- End of function BaseObjArray::BaseObjArray -----------//


//--------- Begin of function BaseObjArray::init ---------//
//
void BaseObjArray::init()
{
}
//----------- End of function BaseObjArray::init -----------//


//--------- Begin of function BaseObjArray::deinit ---------//
//
void BaseObjArray::deinit()
{
#ifdef DEBUG

	for( int i=size() ; i>0 ; i-- )
	{
		if( base_obj_array.is_deleted(i) )
			continue;

		BaseObj* baseObj = base_obj_array[i];

		Unit* unitPtr = baseObj->cast_to_Unit();
		Firm* firmPtr = baseObj->cast_to_Firm();
		Town* townPtr = baseObj->cast_to_Town();
	}

	err_when( packed_size() > 0 );		// the entire array should have deinitialized

#endif

	zap();
}
//----------- End of function BaseObjArray::deinit -----------//


//--------- Begin of function BaseObjArray::add ---------//
//
// Return: <int> the recno of the added object in base_obj_array
//
int BaseObjArray::add(Unit* unitPtr)
{
	unitPtr->obj_type = OBJ_UNIT;
	unitPtr->obj_extern_recno = unitPtr->sprite_recno;

	linkin(&unitPtr);

	unitPtr->base_obj_recno = recno();

	return recno();
}
//----------- End of function BaseObjArray::add -----------//


//--------- Begin of function BaseObjArray::add ---------//
//
// Return: <int> the recno of the added object in base_obj_array
//
int BaseObjArray::add(Firm* firmPtr)
{
	firmPtr->obj_type = OBJ_FIRM;
	firmPtr->obj_extern_recno = firmPtr->firm_recno;

	linkin(&firmPtr);

	firmPtr->base_obj_recno = recno();

	return recno();
}
//----------- End of function BaseObjArray::add -----------//


//--------- Begin of function BaseObjArray::add ---------//
//
// Return: <int> the recno of the added object in base_obj_array
//
int BaseObjArray::add(Town* townPtr)
{
	townPtr->obj_type = OBJ_TOWN;
	townPtr->obj_extern_recno = townPtr->town_recno;

	linkin(&townPtr);

	townPtr->base_obj_recno = recno();

	return recno();
}
//----------- End of function BaseObjArray::add -----------//


//--------- Begin of function BaseObjArray::del ---------//
//
void BaseObjArray::del(int recNo)
{
	BaseObj* baseObjPtr = (BaseObj*) get_ptr(recNo);

	err_when( !baseObjPtr );		// don't delete it as it will be deleted at its derived class level

	linkout(recNo);
}
//----------- End of function BaseObjArray::del -----------//


//--------- Begin of function BaseObjArray::debug_validate ---------//
//
void BaseObjArray::debug_validate()
{
	for( int i=size() ; i>0 ; i-- )
	{
		is_deleted(i);		// is_deleted() already validates something.
	}
}
//----------- End of function BaseObjArray::debug_validate -----------//


#ifdef DEBUG


//------- Begin of function BaseObjArray::operator[] -----//

BaseObj* BaseObjArray::operator[](int recNo)
{
	BaseObj* baseObjPtr = (BaseObj*) get_ptr(recNo);

	if( !baseObjPtr )
		err.run( "BaseObjArray[] is deleted" );

	return baseObjPtr;
}

//--------- End of function BaseObjArray::operator[] ----//

#endif

//------- Begin of function BaseObjArray::is_deleted -----//

int BaseObjArray::is_deleted(int recNo)
{
	BaseObj* baseObj = (BaseObj*) get_ptr(recNo);

	if( !baseObj )
		return 1;

	switch( baseObj->obj_type )
	{
		case OBJ_FIRM:
			return firm_array.is_deleted(baseObj->obj_extern_recno);

		case OBJ_TOWN:
			return town_array.is_deleted(baseObj->obj_extern_recno);

		case OBJ_UNIT:
			return unit_array.is_deleted(baseObj->obj_extern_recno);

		default:
			err_here();
			return 0;
	}
}
//--------- End of function BaseObjArray::is_deleted ----//
