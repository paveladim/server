#ifndef SERVER_H
#define SERVER_H

#include <unordered_map>
#include "definitions.h"
#include "room.h"
#include "client.h"

class server
{
	WSAData _wsa_data;
	SOCKET _server_socket;
	std::unordered_map<std::string, room> _rooms;
	std::list<client> _clients;
public:
	server();
	void list_of_rooms(const std::string& clients_name);
	void create_room(const std::string& command, const std::string& clients_name);
	void enter_room(const std::string& command, const std::string& clients_name);
	void delete_room(const std::string& command);
	void send_to(const SOCKET& sock, const std::string& message);
	~server();
};

#endif
