#include "history_in_file.h"

History_in_file::History_in_file() {
}

History_in_file::History_in_file(const History_in_file& h) {
	_path = h._path;
	_fout.open(_path, std::ofstream::app);
}

History_in_file& History_in_file::operator=(const History_in_file& h) {
	if (this == &h) return *this;
	_path = h._path;
	_fout.open(_path, std::ofstream::app);
	return *this;
}

void History_in_file::start_history(const std::string& name_of_room) {
	_path = name_of_room + ".txt";
	_fout.open(_path, std::ofstream::app);
}

void History_in_file::add_message(const std::string& msg) {
	_fout << msg + "\n";
}

std::string History_in_file::get_history() {
	std::ifstream fin;
	std::string temp;
	std::string info;
	fin.open(_path);

	while (!fin.eof()) {
		temp = "";
		getline(fin, temp);
		info = info + temp + "\n";
	}

	fin.close();
	return info;
}

History_in_file::~History_in_file() {
	_fout.close();
}