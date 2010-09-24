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

// Filename    : OSERIAL.H
// Description : header file of serial number repository


#ifndef __OSERIAL_H
#define __OSERIAL_H

class SerialRepository
{
public:
	long last_serial_id;
	long max_history;
	long cur_history_size;
	long *history_array;
	char file_name[64];

	bool is_persistant() { return file_name[0] != '\0'; }	// save to file every update

public:
	SerialRepository( int maxHistorySize=0, char *fileName=0 );
	~SerialRepository();

	void	init();
	void	deinit();
	int	save();
	int	load();

	// -------- function on serial number -------//

	long	get_serial_number();

	// -------- function on repository -------- //

	void	add(long);
	int	add_unique(long);
	bool	is_exist(long);
	long	count(long);
	bool	remove(long);			// true if found and removed
	bool	remove_first(long);	// remove oldest
	bool	remove_last(long);	// remove latest
};

extern SerialRepository serial_repository;

#endif