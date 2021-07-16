#ifndef HISTORY_H
#define HISTORY_H

#include <queue>
#include "definitions.h"

class history {
	std::queue<std::string> _messages;
	unsigned int _current_depth;
	unsigned int _queue_depth;
public:
	history(const unsigned int& qd = 10);
	void add_message(const std::string& msg);
	std::string get_history();
};

#endif
