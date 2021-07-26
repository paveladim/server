#include "room.h"
#include "client.h"
#include "hoq_factory.h"
#include "hif_factory.h"

Room::Room(const bool& choice, const std::string& name_of_room) : _name_of_room(name_of_room) {
	if (choice) {
		HOQ_factory factory;
		History* h = factory.create_history();
		_history_of_room = std::make_shared<History*>(h);
		(*_history_of_room)->start_history(_name_of_room);
	}
	else {
		HIF_factory factory;
		History* h = factory.create_history();
		_history_of_room = std::make_shared<History*>(h);
		(*_history_of_room)->start_history(_name_of_room);
	}
}

Room::Room(const Room& r) {
	_participants = r._participants;
	_history_of_room = r._history_of_room;
}

Room& Room::operator=(const Room& r) {
	if (this == &r) return *this;
	_participants = r._participants;
	_history_of_room = r._history_of_room;
	return *this;
}

std::string Room::get_time() {
	_tm = time(NULL);
	char temp[256];
	ctime_s(temp, 256, &_tm);
	std::string result(temp);
	return result;
}

void Room::accept_client(Client& c) {
	c.set_room(*this);
	auto ptr = std::make_shared<Client*>(&c);
	_participants.push_back(ptr);
}

void Room::send_to_participants(const std::string& msg, const std::string& name) {
	std::string message = get_time() + name + ": " + msg;
	(*_history_of_room)->add_message(message);
	
	for (auto& elem : _participants)
		if ((*elem)->get_name() != name) send_to((*elem)->get_socket(), message);
}

void Room::kick_user_out(const std::string& name) {
	auto result = std::find_if(_participants.begin(), _participants.end(), [&name](std::shared_ptr<Client*> c) {
		return (*c)->get_name() == name;
	});

	if (result != _participants.end()) {
		send_to_participants(name + " has left the room.", "Server");
		send_to((**result)->get_socket(), "Server: You left the room.");
		_participants.erase(result);
	}
}

void Room::kick_all_users_out() {
	for (auto& elem : _participants)
		(*elem)->unset_room();

	_participants.clear();
}

void Room::get_participants(const std::string& name) {
	std::string info = "Participants:\n";
	for (auto& elem : _participants)
		info = info + (*elem)->get_name() + "\n";
	
	auto result = std::find_if(_participants.begin(), _participants.end(), [&name](std::shared_ptr<Client*> c) {
		return (*c)->get_name() == name;
	});

	if (result != _participants.end()) {
		SOCKET client_socket = (**result)->get_socket();
		send_to(client_socket, info);
	}
}

void Room::get_history(const std::string& name) {
	std::string info = (*_history_of_room)->get_history();

	auto result = std::find_if(_participants.begin(), _participants.end(), [&name](std::shared_ptr<Client*> c) {
		return (*c)->get_name() == name;
	});

	if (result != _participants.end()) {
		SOCKET client_socket = (**result)->get_socket();
		send_to(client_socket, info);
	}
}

void Room::send_to(const SOCKET& sock, const std::string& message) {
	unsigned int msg_size = message.size();
	send(sock, (char*)&msg_size, sizeof(int), NULL);
	send(sock, message.c_str(), msg_size, NULL);
}