#include "client.h"
#include "room.h"
#include "server.h"

client::client(const std::string& name, const SOCKET& sock, server& s) : _name(name),
	_client_socket(sock), _current_room(nullptr), _is_connected(true),
	_current_server(std::make_shared<server*>(&s)), _proc_handler(&client::handler, *this) {
	_proc_handler.detach();
}

client::client(const client& c) {
	_name = c._name;
	_client_socket = c._client_socket;
	_current_room = c._current_room;
	_current_server = c._current_server;
	_is_connected = c._is_connected;
}

client& client::operator=(const client& c) {
	if (this == &c) return *this;
	_name = c._name;
	_client_socket = c._client_socket;
	_current_room = c._current_room;
	_current_server = c._current_server;
	_is_connected = c._is_connected;
	return *this;
}

void client::disconnect() {
	_current_server = nullptr;
	_is_connected = false;
}

void client::set_room(room& r) {
	_current_room = std::make_shared<room*>(&r);
}

void client::leave_room() {
	if (_current_room == nullptr);
	else {
		(*_current_room)->kick_user_out(_name);
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
		(*_current_room)->send_to_participants(s, _name);
	}
}

bool client::send_to_room(const std::string& s) {
	if (_current_room.get() == nullptr) return false;
	else {
		if (s == "@participants") (*_current_room)->get_participants(_name);
		else if (s == "@history") (*_current_room)->get_history(_name);
		else if (s == "@leaveroom") leave_room();
		else return false;
		return true;
	}
}

bool client::send_to_server(const std::string& message) {
	if (_current_server.get() == nullptr) return false;
	else {
		if (message == "@rooms") (*_current_server)->list_of_rooms(*this);
		else if (message.find("@createroom") == 0) (*_current_server)->create_room(message, *this);
		else if (message.find("@enterroom") == 0) (*_current_server)->enter_room(message, *this);
		else if (message.find("@deleteroom") == 0) (*_current_server)->delete_room(message);
		else if (message == "@quit") (*_current_server)->leave_server(*this);
		else if (message == "@help") (*_current_server)->get_help(*this);
		else return false;
		return true;
	}
}

void client::handler() {
	std::string message;
	while (_is_connected) {
		message = receive();
		if (message.find("@") == 0) {
			if ((!send_to_server(message)) && (!send_to_room(message)))
				(*_current_server)->send_to(_client_socket, "Server: Unknown command. Press @help to get more info.");
		}
		else send_to_participants(message);
	}
}

client::~client() {
	
}