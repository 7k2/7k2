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