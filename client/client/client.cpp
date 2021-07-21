#include "client.h"

client::client() : _is_connected(false) {
	WORD dll_version = MAKEWORD(2, 1);

	if (WSAStartup(dll_version, &_wsa_data) == 0)
		std::cout << "Client: Started." << std::endl;

	SOCKADDR_IN addr;
	int addrl = sizeof(addr);
	IN_ADDR inad;
	addr.sin_addr.S_un.S_addr = inet_pton(AF_INET, "127.0.0.1", &inad);
	addr.sin_addr = inad;
	addr.sin_port = htons(1111);
	addr.sin_family = AF_INET;
	_client_socket = socket(AF_INET, SOCK_STREAM, NULL);

	if (connect(_client_socket, (SOCKADDR*)&addr, sizeof(addr)) == 0) {
		std::cout << "Client: Connected." << std::endl;
		_is_connected = true;
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

void client::send_to(const std::string& message) {
	unsigned int msg_size = message.size();
	send(_client_socket, (char*)&msg_size, sizeof(int), NULL);
	send(_client_socket, message.c_str(), msg_size, NULL);
}

void client::handler() {
	std::string message;
	while (_is_connected) {
		message = receive();
		if (message == "@ack") {
			std::cout << "Client: Bye." << std::endl;
			return;
		}
		std::cout << message << std::endl;
	}
}

void client::sender() {
	std::string message;
	while (_is_connected) {
		std::getline(std::cin, message);
		send_to(message);
		if (message == "@quit") {
			_is_connected = false;
			return;
		}
	}
}

client::~client() {
	closesocket(_client_socket);
	WSACleanup();
}