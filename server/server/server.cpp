#include "server.h"

server::server() {
	WORD dll_version = MAKEWORD(2, 1);

	if (WSAStartup(dll_version, &_wsa_data) == 0)
		std::cout << "Server: Started." << std::endl;

	SOCKADDR_IN addr;
	int addrl = sizeof(addr);
	//InetPton(AF_INET, _T("127.0.0.1"), &addr.sin_addr.s_addr);
	addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(1111);
	addr.sin_family = AF_INET;
	_server_socket = socket(AF_INET, SOCK_STREAM, 0);

	bind(_server_socket, (SOCKADDR*)&addr, sizeof(addr));
	listen(_server_socket, SOMAXCONN);

	SOCKET new_connection;
	new_connection = accept(_server_socket, (SOCKADDR*)&addr, &addrl);
	if (new_connection != 0) {
	
	}
}

void server::list_of_rooms(const std::string& clients_name) {
	std::string info = "Rooms:\n";
	for (auto& elem : _rooms)
		info = info + elem.first + "\n";

	auto result = std::find_if(_clients.begin(), _clients.end(), [&clients_name](client c) {
		return c.get_name() == clients_name;
	});

	if (result != _clients.end()) {
		SOCKET client_socket = result->get_socket();
		send_to(client_socket, info);
	}
}

void server::create_room(const std::string& command, const std::string& clients_name) {

}

void server::enter_room(const std::string& command, const std::string& clients_name) {

}

void server::delete_room(const std::string& command) {

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