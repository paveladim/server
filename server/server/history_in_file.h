#pragma once

#include "history.h"
#include <fstream>
#include <ostream>

class History_in_file : public History
{
	std::string _path;
	std::ofstream _fout;
public:
	History_in_file();
	History_in_file(const History_in_file& h);
	void start_history(const std::string& name_of_room);
	History_in_file& operator=(const History_in_file& h);
	virtual void add_message(const std::string& msg);
	virtual std::string get_history();
	~History_in_file();
};

