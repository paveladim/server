#include "logger.h"

Logger::Logger() {
	_path = "logger.txt";
	_fout.open(_path);
	_fout << "LOG\n";
}

std::string Logger::get_time() {
	_tm = time(NULL);
	char temp[256];
	ctime_s(temp, 256, &_tm);
	std::string result(temp);
	return result;
}

void Logger::info(const std::string& message) {
	_fout << "INFO: " + message + " " + get_time();
}

void Logger::debug(const std::string& message) {
	_fout << "DEBUG: " + message + " " + get_time();
}

void Logger::warning(const std::string& message) {
	_fout << "WARNING: " + message + " " + get_time();
}

void Logger::error(const std::string& message) {
	_fout << "ERROR: " + message + " " + get_time();
}

void Logger::fatal(const std::string& message) {
	_fout << "FATAL: " + message + " " + get_time();
}

Logger::~Logger() {
	_fout.close();
}