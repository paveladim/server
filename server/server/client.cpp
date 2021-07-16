#include "client.h"
#include "room.h"
#include "server.h"

client::client() {
	_is_connected = false;
}

client::client(const std::string& name, const SOCKET& sock, const server& s) : _name(name),
	_client_socket(sock), _current_room(nullptr), _is_connected(false) {
	_current_server = std::make_shared<server>(s);
}

client::client(const client& c) {
	_name = c._name;
	_client_socket = c._client_socket;
	_current_room = c._current_room;
	_current_server = c._current_server;
	_is_connected = c._is_connected;
}

void client::set_room(const room& r) {
	_current_room = std::make_shared<room>(r);
}

void client::leave_room() {
	if (_current_room == nullptr);
	else {
		_current_room.get()->kick_user_out(_name);
		_current_room = nullptr;
	}
}

std::string client::receive() {
	unsigned int msg_size = 0;
	recv(_client_socket, (char*)&msg_size, sizeof(int), 0);
	char* msg = new char[msg_size + 1];
	msg[msg_size] = '\0';
	recv(_client_socket, msg, msg_size, 0);
	return std::string(msg);
}

void client::send_to_participants(const std::string& s) {
	if (_current_room == nullptr);
	else {
		_current_room.get()->send_to_participants(s, _name);
	}
}

void client::send_to_room(const std::string& s) {
	if (_current_room == nullptr);
	else {
		if (s == "@participants") _current_room.get()->get_participants(_name);
		else if (s == "@history") _current_room.get()->get_history(_name);
		else if (s == "@leaveroom") leave_room();
	}
}

void client::send_to_server(const std::string& s) {
	if (_current_server == nullptr);
	else {
		if (s == "@rooms") _current_server.get()->list_of_rooms(_name);
		else if (s.find("@createroom") == 0) _current_server.get()->create_room(s, _name);
		else if (s.find("@enterroom") == 0) _current_server.get()->enter_room(s, _name);
	}
}

void client::handler() {
	std::string message;
	while (_is_connected) {
		message = receive();
		if (message.find("@") == 0) {
			send_to_room(message);
			send_to_server(message);
		}
		else send_to_participants(message);
	}
}