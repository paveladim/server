#include "history.h"

history::history(const unsigned int& qd) : _queue_depth(qd), _current_depth(0) {}

void history::add_message(const std::string& msg) {
	if (_current_depth == _queue_depth) {
		_messages.pop();
		_messages.push(msg);
	}
	else {
		_messages.push(msg);
		++_current_depth;
	}
}

std::string history::get_history() {
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