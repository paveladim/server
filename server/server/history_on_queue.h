#ifndef HISTORY_ON_QUEUE_H
#define HISTORY_ON_QUEUE_H

#include <queue>
#include "history.h"

class History_on_queue : public History
{
	std::queue<std::string> _messages;
	unsigned int _current_depth;
	unsigned int _queue_depth;
public:
	History_on_queue(const unsigned int& qd);
	History_on_queue(const History_on_queue& h);
	History_on_queue& operator=(const History_on_queue& h);
	virtual void add_message(const std::string& msg);
	virtual std::string get_history();
};

#endif