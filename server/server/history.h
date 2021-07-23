#ifndef HISTORY_H
#define HISTORY_H

#include "definitions.h"

class History {
public:
	History() {};
	History(const History& h) = default;
	History& operator=(const History& h) = default;
	virtual void add_message(const std::string& msg) = 0;
	virtual std::string get_history() = 0;
};

#endif
