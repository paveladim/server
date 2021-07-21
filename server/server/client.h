#ifndef CLIENT_H
#define CLIENT_H

#include "definitions.h"

class server;
class room;

class client
{
	std::string _name;
	SOCKET _client_socket;
	std::shared_ptr<room*> _current_room;
	std::shared_ptr<server*> _current_server;
	bool _is_connected;
	std::thread _proc_handler;
public:
	client() : _is_connected(false), _client_socket(0) {}
	client(const std::string& name, const SOCKET& sock, server& s);
	client(const client& c);
	client& operator=(const client& c);
	void disconnect();
	void unset_room() { _current_room = nullptr; }
	void set_room(room& r);
	void leave_room();
	const std::string& get_name() const { return _name; }
	const SOCKET& get_socket() const { return _client_socket; }
	std::string receive();
	void send_to_participants(const std::string& s);
	bool send_to_room(const std::string& s);
	bool send_to_server(const std::string& s);
	void handler();
	~client();
};

#endif