#include "server.h"

server::server() {
	WORD dll_version = MAKEWORD(2, 1);

	if (WSAStartup(dll_version, &_wsa_data) == 0)
		std::cout << "Server: Started." << std::endl;

	SOCKADDR_IN addr;
	int addrl = sizeof(addr);
	// addr.sin_addr.S_un.S_addr = inet_pton(AF_INET, "127.0.0.1", &addr);
	addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(1111);
	addr.sin_family = AF_INET;
	_server_socket = socket(AF_INET, SOCK_STREAM, NULL);

	bind(_server_socket, (SOCKADDR*)&addr, sizeof(addr));
	listen(_server_socket, SOMAXCONN);

	SOCKET new_con;
	while (new_con = accept(_server_socket, (SOCKADDR*)&addr, &addrl)) {
		if (new_con != 0) {
			std::cout << "Success" << std::endl;
			send_to(new_con, "Server: Hello!\n");
			register_user(new_con);
			std::cout << "Server: Connected " + _clients.back().get_name() + ".\n";
			_clients.back().handler();
		}
	}
}

server::server(const server& s) {
	_wsa_data = s._wsa_data;
	_server_socket = s._server_socket;
	_rooms = s._rooms;
	_clients = s._clients;
}

server& server::operator=(const server& s) {
	if (this == &s) return *this;
	_wsa_data = s._wsa_data;
	_server_socket = s._server_socket;
	_rooms = s._rooms;
	_clients = s._clients;
}

bool server::is_name_unique(std::string& clients_name) {
	for (auto& elem : _clients)
		if (clients_name == elem.get_name()) return false;
	
	return true;
}

void server::register_user(const SOCKET& sock) {
	send_to(sock, "Introduce yourself.");
	std::string name = receive(sock);

	while (!is_name_unique(name)) {
		send_to(sock, "Server: Name is not unique. Try again.\n");
		name = receive(sock);
	}

	client new_client(name, sock, *this);
	_clients.push_back(new_client);
	send_to(sock, "Welcome, " + name + "!");
}

std::string server::receive(const SOCKET& sock) {
	unsigned int msg_size = 0;
	recv(sock, (char*)&msg_size, sizeof(int), 0);
	char* msg = new char[msg_size + 1];
	msg[msg_size] = '\0';
	recv(sock, msg, msg_size, 0);
	return std::string(msg);
}

void server::list_of_rooms(const std::string& clients_name) {
	std::string info;
	if (_rooms.size() == 0) {
		info = "Server: There's no room.\n";
	}
	else {
		info = "Rooms:\n";
		for (auto& elem : _rooms)
			info = info + elem.first + "\n";
	}

	auto result = std::find_if(_clients.begin(), _clients.end(), [&clients_name](client c) {
		return c.get_name() == clients_name;
	});

	if (result != _clients.end()) {
		SOCKET client_socket = result->get_socket();
		send_to(client_socket, info);
	}
}

void server::create_room(const std::string& command, const std::string& clients_name) {
	auto pos = find(command.begin(), command.end(), ' ');
	std::string name_of_room;
	if ((pos != command.end()) && (pos + 1 != command.end())) {
		for (auto it = pos + 1; it < command.end(); ++it) name_of_room.push_back(*it);
		_rooms[name_of_room] = *(new room(10));
	}
}

void server::enter_room(const std::string& command, const std::string& clients_name) {
	auto pos = find(command.begin(), command.end(), ' ');
	std::string name_of_room;
	if ((pos != command.end()) && (pos + 1 != command.end())) {
		for (auto it = pos + 1; it < command.end(); ++it) name_of_room.push_back(*it);
	}

	auto result = std::find_if(_clients.begin(), _clients.end(), [&clients_name](client c) {
		return c.get_name() == clients_name;
	});

	if ((result != _clients.end()) && (_rooms.find(name_of_room) != _rooms.end()))
		_rooms[name_of_room].accept_client(*result);
}

void server::delete_room(const std::string& command) {

}

void server::leave_server(const std::string& clients_name) {
	auto result = std::find_if(_clients.begin(), _clients.end(), [&clients_name](client c) {
		return c.get_name() == clients_name;
	});

	if (result != _clients.end()) {
		result->leave_room();
		result->disconnect();
	}
}

void server::send_to(const SOCKET& sock, const std::string& message) {
	unsigned int msg_size = message.size();
	send(sock, (char*)&msg_size, sizeof(int), NULL);
	send(sock, message.c_str(), msg_size, NULL);
}

server::~server() {
	closesocket(_server_socket);
	WSACleanup();
}