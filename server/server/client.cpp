#include "client.h"
#include "room.h"
#include "server.h"

Client::Client(const std::string& name, const SOCKET& sock, Server& s) : _name(name),
	_client_socket(sock), _current_room(nullptr), _is_connected(true),
	_current_server(std::make_shared<Server*>(&s)), _proc_handler(&Client::handler, *this) {
	_proc_handler.detach();
}

Client::Client(const Client& c) {
	_name = c._name;
	_client_socket = c._client_socket;
	_current_room = c._current_room;
	_current_server = c._current_server;
	_is_connected = c._is_connected;
}

Client& Client::operator=(const Client& c) {
	if (this == &c) return *this;
	_name = c._name;
	_client_socket = c._client_socket;
	_current_room = c._current_room;
	_current_server = c._current_server;
	_is_connected = c._is_connected;
	return *this;
}

void Client::disconnect() {
	_current_server = nullptr;
	_is_connected = false;
}

void Client::set_room(Room& r) {
	_current_room = std::make_shared<Room*>(&r);
}

void Client::leave_room() {
	if (_current_room == nullptr);
	else {
		(*_current_room)->kick_user_out(_name);
		_current_room = nullptr;
	}
}

std::string Client::receive() const {
	unsigned int msg_size = 0;
	recv(_client_socket, (char*)&msg_size, sizeof(int), 0);
	char* msg = new char[msg_size + 1];
	msg[msg_size] = '\0';
	recv(_client_socket, msg, msg_size, 0);
	return std::string(msg);
}

void Client::send_to_participants(const std::string& s) {
	if (_current_room == nullptr) {
		(*_current_server)->send_to(_client_socket, "Server: You are not connected to any room");
	}
	else {
		(*_current_room)->send_to_participants(s, _name);
	}
}

bool Client::send_to_room(const std::string& s) {
	if (_current_room.get() == nullptr) return false;
	else {
		if (s == "@participants") (*_current_room)->get_participants(_name);
		else if (s == "@history") (*_current_room)->get_history(_name);
		else if (s == "@leaveroom") leave_room();
		else return false;
		return true;
	}
}

bool Client::send_to_server(const std::string& message) {
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

void Client::handler() {
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

Client::~Client() {
	
}