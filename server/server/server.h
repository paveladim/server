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
	server(const server& s);
	server& operator=(const server& s);
	bool is_name_unique(std::string& clients_name);
	void register_user(const SOCKET& sock);
	std::string receive(const SOCKET& sock);
	void list_of_rooms(const client& c);
	void create_room(const std::string& command, const client& c);
	void enter_room(const std::string& command, client& c);
	void delete_room(const std::string& command);
	void leave_server(client& c);
	void get_help(const client& c);
	void send_to(const SOCKET& sock, const std::string& message);
	~server();
};

#endif
