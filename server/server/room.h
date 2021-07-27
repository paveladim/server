#ifndef ROOM_H
#define ROOM_H

#include "definitions.h"

class History;
class History_factory;
class Client;

class Room
{
	std::list<std::shared_ptr<Client*>> _participants;
	std::shared_ptr<History*> _history_of_room;
	time_t _tm;
	std::string _name_of_room;
public:
	Room() : _tm(time(NULL)) {}
	Room(const bool& choice, const std::string& name);
	Room(const Room& r);
	Room& operator=(const Room& r);
	std::string get_time();
	std::shared_ptr<History*> get_room_hist() { return _history_of_room; };
	void accept_client(Client& c);
	void send_to_participants(const std::string& msg, const std::string& name);
	void kick_user_out(const std::string& name);
	void kick_all_users_out();
	void get_participants(const std::string& name);
	void get_history(const std::string& name);
	void send_to(const SOCKET& sock, const std::string& message);
	~Room() {}
};

#endif