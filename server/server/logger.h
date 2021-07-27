#ifndef LOGGER_H
#define LOGGER_H

#include "definitions.h"
#include <fstream>
#include <ostream>

class Logger
{
	static Logger* _logger;
	std::string _path;
	std::ofstream _fout;
	time_t _tm;
	Logger();
public:
	Logger(const Logger& l) = delete;
	Logger& operator=(const Logger& l) = delete;
	static Logger* get_logger();
	std::string get_time();
	void info(const std::string& message);
	void debug(const std::string& message);
	void warning(const std::string& message);
	void error(const std::string& message);
	void fatal(const std::string& message);
	~Logger();
};

#endif
