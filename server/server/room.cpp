#include "room.h"
#include "client.h"

room::room(const unsigned int& qd) : _history_of_room(qd) {}

room::room(const room& r) {
	_participants = r._participants;
	_history_of_room = r._history_of_room;
}

room& room::operator=(const room& r) {
	if (this == &r) return *this;
	_participants = r._participants;
	_history_of_room = r._history_of_room;
	return *this;
}

void room::accept_client(client& c) {
	c.set_room(*this);
	auto ptr = std::make_shared<client*>(&c);
	_participants.push_back(ptr);
}

void room::send_to_participants(const std::string& msg, const std::string& name) {
	std::string message = name + ": " + msg;
	_history_of_room.add_message(message);
	
	for (auto& elem : _participants)
		if ((*elem)->get_name() != name) send_to((*elem)->get_socket(), message);
}

void room::kick_user_out(const std::string& name) {
	auto result = std::find_if(_participants.begin(), _participants.end(), [&name](std::shared_ptr<client*> c) {
		return (*c)->get_name() == name;
	});

	if (result != _participants.end()) {
		send_to_participants(name + " has left the room.", "Server");
		send_to((**result)->get_socket(), "Server: You left the room.");
		_participants.erase(result);
	}
}

void room::kick_all_users_out() {
	for (auto& elem : _participants)
		(*elem)->unset_room();

	_participants.clear();
}

void room::get_participants(const std::string& name) {
	std::string info = "Participants:\n";
	for (auto& elem : _participants)
		info = info + (*elem)->get_name() + "\n";
	
	auto result = std::find_if(_participants.begin(), _participants.end(), [&name](std::shared_ptr<client*> c) {
		return (*c)->get_name() == name;
	});

	if (result != _participants.end()) {
		SOCKET client_socket = (**result)->get_socket();
		send_to(client_socket, info);
	}
}

void room::get_history(const std::string& name) {
	std::string info = _history_of_room.get_history();

	auto result = std::find_if(_participants.begin(), _participants.end(), [&name](std::shared_ptr<client*> c) {
		return (*c)->get_name() == name;
	});

	if (result != _participants.end()) {
		SOCKET client_socket = (**result)->get_socket();
		send_to(client_socket, info);
	}
}

void room::send_to(const SOCKET& sock, const std::string& message) {
	unsigned int msg_size = message.size();
	send(sock, (char*)&msg_size, sizeof(int), NULL);
	send(sock, message.c_str(), msg_size, NULL);
}