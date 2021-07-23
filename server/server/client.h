#ifndef CLIENT_H
#define CLIENT_H

#include "definitions.h"

class Server;
class Room;

class Client
{
	std::string _name;
	SOCKET _client_socket;
	std::shared_ptr<Room*> _current_room;
	std::shared_ptr<Server*> _current_server;
	bool _is_connected;
	std::thread _proc_handler;
public:
	Client() : _is_connected(false), _client_socket(0) {}
	Client(const std::string& name, const SOCKET& sock, Server& s);
	Client(const Client& c);
	Client& operator=(const Client& c);
	void disconnect();
	void unset_room() { _current_room = nullptr; }
	void set_room(Room& r);
	void leave_room();
	const std::string& get_name() const { return _name; }
	const SOCKET& get_socket() const { return _client_socket; }
	std::string receive() const;
	void send_to_participants(const std::string& s);
	bool send_to_room(const std::string& s);
	bool send_to_server(const std::string& s);
	void handler();
	~Client();
};

#endif