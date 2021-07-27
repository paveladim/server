#ifndef SERVER_H
#define SERVER_H

#include <unordered_map>
#include "definitions.h"
#include "room.h"
#include "client.h"
#include "logger.h"

class Server
{
	WSAData _wsa_data;
	SOCKET _server_socket;
	std::unordered_map<std::string, Room> _rooms;
	std::list<Client> _clients;
	Logger* _logger;
public:
	Server();
	Server(const Server& s);
	Server& operator=(const Server& s);
	bool is_name_unique(std::string& clients_name);
	void register_user(const SOCKET& sock);
	std::string receive(const SOCKET& sock);
	void list_of_rooms(const Client& c);
	void create_room(const std::string& command, const Client& c);
	void enter_room(const std::string& command, Client& c);
	void delete_room(const std::string& command);
	void leave_server(Client& c);
	void get_help(const Client& c);
	void send_to(const SOCKET& sock, const std::string& message);
	~Server();
};

#endif
