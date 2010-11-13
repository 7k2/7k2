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

// Filename    : OSERIAL.CPP
// Description : Serial number repository , acquire, store, and checking

#include <OSERIAL.H>
#include <ALL.H>


// ------ begin of function SerialRepository::SerialRepository --------//
//
SerialRepository::SerialRepository( int maxHistorySize, char *fileName )
{
	last_serial_id = 0;
	max_history = maxHistorySize;
	cur_history_size = 0;

	if( maxHistorySize > 0 )
		history_array = (long *) mem_add( sizeof(long) * maxHistorySize );
	else
		history_array = NULL;

	if( fileName )
	{
		strncpy( file_name, fileName, sizeof(file_name)-1 );
		file_name[sizeof(file_name)-1] = '\0';
	}
	else
	{
		file_name[0] = '\0';
	}
}
// ------ end of function SerialRepository::SerialRepository --------//


// ------ begin of function SerialRepository::~SerialRepository --------//
//
SerialRepository::~SerialRepository()
{
	deinit();
}
// ------ end of function SerialRepository::~SerialRepository --------//


// ------ begin of function SerialRepository::init --------//
// reset serial
void SerialRepository::init()
{
	last_serial_id = 0;
	if( history_array )
		memset( history_array, 0, sizeof(history_array[0]) * max_history);
	cur_history_size = 0;
}
// ------ end of function SerialRepository::init --------//


// ------ begin of function SerialRepository::deinit --------//
//
void SerialRepository::deinit()
{
	if( history_array )
	{
		mem_del(history_array);
		history_array = NULL;
	}
}
// ------ end of function SerialRepository::deinit --------//


// ------ begin of function SerialRepository::save --------//
//
int SerialRepository::save()
{
	if( !is_persistant() )
		return 0;

	int rc;

	File file;
	file.file_create( file_name );

	file.file_put_long(last_serial_id);
	file.file_put_long(max_history);
	file.file_put_long(cur_history_size);
	file.file_put_long(0);		// dummy
	file.file_put_long(0);
	file.file_put_long(0);
	file.file_put_long(0);
	file.file_put_long(0);

	rc = file.file_write( history_array, sizeof(history_array[0]) * cur_history_size );

	file.file_close();
	return rc;
}
// ------ end of function SerialRepository::save --------//


// ------ begin of function SerialRepository::load --------//
//
int SerialRepository::load()
{
	if( !is_persistant() )
		return 0;

	if( !m.is_file_exist(file_name) )
		return 0;

	int rc;

	File file;
	file.file_open( file_name );

	last_serial_id = file.file_get_long();
	max_history = file.file_get_long();
	history_array = (long *)mem_resize( history_array, sizeof(history_array[0]) * max_history );

	cur_history_size = file.file_get_long();
	(void) file.file_get_long();		// dummy
	(void) file.file_get_long();
	(void) file.file_get_long();
	(void) file.file_get_long();
	(void) file.file_get_long();

	rc = file.file_read( history_array, sizeof(history_array[0]) * cur_history_size );

	file.file_close();
	return rc;
}
// ------ end of function SerialRepository::load --------//


// ------ begin of function SerialRepository::get_serial_number --------//
//
long SerialRepository::get_serial_number()
{
	++last_serial_id;

	save();

	return last_serial_id;
}
// ------ end of function SerialRepository::get_serial_number --------//


// ------ begin of function SerialRepository::add --------//
//
void SerialRepository::add(long num)
{
	if( max_history <= 0 )
		return;

	// free space
	if( cur_history_size >= max_history )
	{
		memmove( history_array, history_array+1, (max_history-1) * sizeof(history_array[0]) );
		cur_history_size = max_history - 1;
	}

	history_array[cur_history_size] = num;
	cur_history_size++;

	save();
}
// ------ end of function SerialRepository::add --------//


// ------ begin of function SerialRepository::add_unique --------//
// return true if newly added
int SerialRepository::add_unique(long num)
{
	if( max_history <= 0 )
		return 0;

	if( !is_exist(num) )
	{
		add( num );
		return 1;
	}
	else
	{
		return 0;
	}
}
// ------ end of function SerialRepository::add_unique --------//


// ------ begin of function SerialRepository::is_exist --------//
//
bool SerialRepository::is_exist(long num)
{
	if( max_history <= 0 )
		return 0;

	for( register long i = cur_history_size-1; i >= 0 && history_array[i] != num; --i );

	return i >= 0;
}
// ------ end of function SerialRepository::is_exist --------//


// ------ begin of function SerialRepository::count --------//
//
long SerialRepository::count(long num)
{
	long c = 0;
	for( register long i = cur_history_size-1; i >= 0 ; --i )
	{
		if( history_array[i] == num )
			++c;
	}

	return c;
}
// ------ end of function SerialRepository::count --------//


// ------ begin of function SerialRepository::remove --------//
//
bool SerialRepository::remove(long num)
{
	if( max_history <= 0 )
		return false;

	bool flag = false;

	for( register long i = cur_history_size-1; i >= 0 ; --i )
	{
		if( history_array[i] == num )
		{
			m.del_array_rec (history_array, cur_history_size, sizeof(history_array[0]), i+1 );
			--cur_history_size;
			flag = true;
		}
	}

	if( flag )
		save();

	return flag;
}
// ------ end of function SerialRepository::remove --------//


// ------ begin of function SerialRepository::remove_first --------//
//
bool SerialRepository::remove_first(long num)
{
	if( max_history <= 0 )
		return false;

	for( register long i = 0; i < cur_history_size; ++i )
	{
		if( history_array[i] == num )
		{
			m.del_array_rec (history_array, cur_history_size, sizeof(history_array[0]), i+1 );
			--cur_history_size;
			// i--;
			save();
			return true;
		}
	}

	return false;
}
// ------ end of function SerialRepository::remove_first --------//


// ------ begin of function SerialRepository::remove_last --------//
//
bool SerialRepository::remove_last(long num)
{
	if( max_history <= 0 )
		return false;

	for( register long i = cur_history_size-1; i >= 0 ; --i )
	{
		if( history_array[i] == num )
		{
			m.del_array_rec (history_array, cur_history_size, sizeof(history_array[0]), i+1 );
			--cur_history_size;
			save();
			return true;
		}
	}

	return false;
}
// ------ end of function SerialRepository::remove_last --------//
