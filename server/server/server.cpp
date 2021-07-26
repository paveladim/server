#include "server.h"

Server::Server() {
	WORD dll_version = MAKEWORD(2, 1);

	if (WSAStartup(dll_version, &_wsa_data) == 0)
		_logger.info("Started");

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
			_logger.info("Got new connection");
			send_to(new_con, "Server: Hello!\n");
			register_user(new_con);
		}
	}
}

Server::Server(const Server& s) {
	_wsa_data = s._wsa_data;
	_server_socket = s._server_socket;
	_rooms = s._rooms;
	_clients = s._clients;
}

Server& Server::operator=(const Server& s) {
	if (this == &s) return *this;
	_wsa_data = s._wsa_data;
	_server_socket = s._server_socket;
	_rooms = s._rooms;
	_clients = s._clients;
	return *this;
}

bool Server::is_name_unique(std::string& clients_name) {
	for (auto& elem : _clients)
		if (clients_name == elem.get_name()) return false;
	
	return true;
}

void Server::register_user(const SOCKET& sock) {
	send_to(sock, "Introduce yourself.");
	std::string name = receive(sock);

	while ((!is_name_unique(name)) || (name.find("@") == 0)) {
		if (!is_name_unique(name)) {
			send_to(sock, "Server: Name is not unique. Try again.");
			_logger.warning("User entered a non-unique name");
		}
		else if (name.find("@") == 0) {
			send_to(sock, "Server: Name cannot start with @. Try again.");
			_logger.warning("User entered a service command");
		}
		name = receive(sock);
	}

	send_to(sock, "Server: Welcome, " + name + "!");
	send_to(sock, "Server: Press @help to get more information.");
	_logger.info("Connected " + name);
	_clients.push_back(Client(name, sock, *this));
}

std::string Server::receive(const SOCKET& sock) {
	unsigned int msg_size = 0;
	recv(sock, (char*)&msg_size, sizeof(int), 0);
	char* msg = new char[msg_size + 1];
	msg[msg_size] = '\0';
	recv(sock, msg, msg_size, 0);
	return std::string(msg);
}

void Server::list_of_rooms(const Client& c) {
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

void Server::create_room(const std::string& command, const Client& c) {
	auto pos = find(command.begin(), command.end(), ' ');
	std::string name_of_room;
	if ((pos != command.end()) && (pos + 1 != command.end())) {
		for (auto it = pos + 1; it < command.end(); ++it) name_of_room.push_back(*it);
		
		if (name_of_room.find("@") == 0) {
			send_to(c.get_socket(), "Server: Name cannot start with @. Try again.");
			_logger.warning("User entered a service command");
			return;
		}
		else if (_rooms.find(name_of_room) == _rooms.end()) {
			send_to(c.get_socket(), "Server: Choose a way to store messages : On server or In file.");
			std::string way_of_storing = c.receive();
			if (way_of_storing == "On server") {
				send_to(c.get_socket(), "Server: Room " + name_of_room + " has been created.");
				_logger.info("Room " + name_of_room + " has been created");
				Room r(true, name_of_room);
				_rooms[name_of_room] = r;
			}
			else if (way_of_storing == "In file") {
				send_to(c.get_socket(), "Server: Room " + name_of_room + " has been created.");
				_logger.info("Room " + name_of_room + " has been created");
				Room r(false, name_of_room);
				_rooms[name_of_room] = r;
			}
			else {
				send_to(c.get_socket(), "Server: Unknown choice. Try again.");
				_logger.warning("Unknown storage method");
				return;
			}
		}
		else {
			send_to(c.get_socket(), "Server: Name is not unique. Try again.");
			_logger.warning("User entered a non-unique name");
		}
	}
}

void Server::enter_room(const std::string& command, Client& c) {
	auto pos = find(command.begin(), command.end(), ' ');
	std::string name_of_room;
	if ((pos != command.end()) && (pos + 1 != command.end())) {
		for (auto it = pos + 1; it < command.end(); ++it) name_of_room.push_back(*it);
	}

	if (_rooms.find(name_of_room) != _rooms.end()) {
		_rooms[name_of_room].accept_client(c);
		send_to(c.get_socket(), "Server: You entered the room " + name_of_room + ".");
		_rooms[name_of_room].send_to_participants(c.get_name() + " has joined to room.", "Server");
		_logger.info(c.get_name() + " has joined to room");
	}
	else {
		send_to(c.get_socket(), "Server: Room does not exist.");
		_logger.warning("Room was not found");
	}
}

void Server::delete_room(const std::string& command) {
	auto pos = find(command.begin(), command.end(), ' ');
	std::string name_of_room;
	if ((pos != command.end()) && (pos + 1 != command.end())) {
		for (auto it = pos + 1; it < command.end(); ++it) name_of_room.push_back(*it);
	}

	std::string message = "Room " + name_of_room + " has been closed.";
	_logger.info("Room " + name_of_room + " has been closed");
	_rooms[name_of_room].send_to_participants(message, "Server");
	_rooms[name_of_room].kick_all_users_out();
	_rooms.erase(name_of_room);
}

void Server::leave_server(Client& c) {
	c.leave_room();
	c.disconnect();

	send_to(c.get_socket(), "@ack");
	_logger.info("Disconnected " + c.get_name());
	
	auto position = std::find_if(_clients.begin(), _clients.end(), [&c](Client cur) {
		return c.get_name() == cur.get_name();
	});
	_clients.erase(position);
}

void Server::get_help(const Client& c) {
	std::string info = "\t\t\t\tHelp:\n";
	info += "@rooms 				- get information about rooms on server\n";
	info += "@createroom <name>		- create room\n";
	info += "@enterroom <name>		- enter the room with the chosen name\n";
	info += "@participants			- request a list of participants in room\n";
	info += "@history			- request room message history\n";
	info += "@leaveroom 			- leave the room where you at\n";
	info += "@deleteroom <name>		- delete the room with the chosen name\n";
	info += "@help				- get help\n";
	info += "@quit				- end session";

	send_to(c.get_socket(), info);
}

void Server::send_to(const SOCKET& sock, const std::string& message) {
	unsigned int msg_size = message.size();
	send(sock, (char*)&msg_size, sizeof(int), NULL);
	send(sock, message.c_str(), msg_size, NULL);
}

Server::~Server() {
	_logger.info("Server is shutting down");
	for (auto& elem : _clients)
		leave_server(elem);

	_clients.clear();
	_rooms.clear();
	closesocket(_server_socket);
	WSACleanup();
}