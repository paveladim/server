#include "server.h"

server::server() {
	WORD dll_version = MAKEWORD(2, 1);

	if (WSAStartup(dll_version, &_wsa_data) == 0)
		std::cout << "Server: Started." << std::endl;

	SOCKADDR_IN addr;
	int addrl = sizeof(addr);
	IN_ADDR inad;
	addr.sin_addr.S_un.S_addr = inet_pton(AF_INET, "127.0.0.1", &inad);
	addr.sin_addr = inad;
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
	return *this;
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

	send_to(sock, "Welcome, " + name + "!");
	send_to(sock, "Server: Press @help to get more information.");
	std::cout << "Server: Connected " + name + ".\n";
	_clients.push_back(client(name, sock, *this));
}

std::string server::receive(const SOCKET& sock) {
	unsigned int msg_size = 0;
	recv(sock, (char*)&msg_size, sizeof(int), 0);
	char* msg = new char[msg_size + 1];
	msg[msg_size] = '\0';
	recv(sock, msg, msg_size, 0);
	return std::string(msg);
}

void server::list_of_rooms(const client& c) {
	std::string info;
	if (_rooms.size() == 0) {
		info = "Server: There's no room.";
	}
	else {
		info = "Rooms:\n";
		for (auto& elem : _rooms)
			info = info + elem.first + "\n";
	}

	SOCKET client_socket = c.get_socket();
	send_to(client_socket, info);
}

void server::create_room(const std::string& command, const client& c) {
	auto pos = find(command.begin(), command.end(), ' ');
	std::string name_of_room;
	if ((pos != command.end()) && (pos + 1 != command.end())) {
		for (auto it = pos + 1; it < command.end(); ++it) name_of_room.push_back(*it);
		
		if (_rooms.find(name_of_room) == _rooms.end()) {
			send_to(c.get_socket(), "Server: Room " + name_of_room + " has been created.");
			room r(10);
			_rooms[name_of_room] = r;
		}
		else send_to(c.get_socket(), "Server: Name is not unique. Try again.");
	}
}

void server::enter_room(const std::string& command, client& c) {
	auto pos = find(command.begin(), command.end(), ' ');
	std::string name_of_room;
	if ((pos != command.end()) && (pos + 1 != command.end())) {
		for (auto it = pos + 1; it < command.end(); ++it) name_of_room.push_back(*it);
	}

	if (_rooms.find(name_of_room) != _rooms.end()) {
		_rooms[name_of_room].accept_client(c);
		send_to(c.get_socket(), "Server: You entered the room " + name_of_room + ".");
		_rooms[name_of_room].send_to_participants(c.get_name() + " has joined to room.", "Server");
	}
	else send_to(c.get_socket(), "Server: Room does not exist.");
}

void server::delete_room(const std::string& command) {
	auto pos = find(command.begin(), command.end(), ' ');
	std::string name_of_room;
	if ((pos != command.end()) && (pos + 1 != command.end())) {
		for (auto it = pos + 1; it < command.end(); ++it) name_of_room.push_back(*it);
	}

	std::string message = "Room " + name_of_room + " has been closed.";
	_rooms[name_of_room].send_to_participants(message, "Server");
	_rooms[name_of_room].kick_all_users_out();
	_rooms.erase(name_of_room);
}

void server::leave_server(client& c) {
	c.leave_room();
	c.disconnect();

	send_to(c.get_socket(), "@ack");
	
	auto position = std::find_if(_clients.begin(), _clients.end(), [&c](client cur) {
		return c.get_name() == cur.get_name();
	});
	_clients.erase(position);
}

void server::get_help(const client& c) {
	std::string info = "\tHelp:\n";
	info += "@rooms 				- get information about rooms on server\n";
	info += "@createroom <name>		- create room\n";
	info += "@enterroom <name>		- enter the room with the chosen name\n";
	info += "@participants			- request a list of participants in room\n";
	info += "@leaveroom 			- leave the room where you at\n";
	info += "@deleteroom <name>		- delete the room with the chosen name\n";
	info += "@help					- get help\n";
	info += "@quit					- end session";

	send_to(c.get_socket(), info);
}

void server::send_to(const SOCKET& sock, const std::string& message) {
	unsigned int msg_size = message.size();
	send(sock, (char*)&msg_size, sizeof(int), NULL);
	send(sock, message.c_str(), msg_size, NULL);
}

server::~server() {
	for (auto& elem : _clients)
		elem.leave_room();

	_clients.clear();
	_rooms.clear();
	closesocket(_server_socket);
	WSACleanup();
}