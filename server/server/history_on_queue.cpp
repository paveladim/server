#include "history_on_queue.h"

History_on_queue::History_on_queue(const unsigned int& qd) : _queue_depth(qd), _current_depth(0) {}

History_on_queue::History_on_queue(const History_on_queue& h) {
	_messages = h._messages;
	_current_depth = h._current_depth;
	_queue_depth = h._queue_depth;
}

History_on_queue& History_on_queue::operator=(const History_on_queue& h) {
	if (this == &h) return *this;
	_messages = h._messages;
	_current_depth = h._current_depth;
	_queue_depth = h._queue_depth;
	return *this;
}

void History_on_queue::add_message(const std::string& msg) {
	if (_current_depth == _queue_depth) {
		_messages.pop();
		_messages.push(msg);
	}
	else {
		_messages.push(msg);
		++_current_depth;
	}
}

std::string History_on_queue::get_history() {
	std::string res;
	std::string temp;

	for (unsigned int i = 0; i < _current_depth; ++i) {
		temp = _messages.front();
		_messages.pop();
		res = res + temp + "\n";
		_messages.push(temp);
	}

	return res;
}
