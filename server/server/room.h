#ifndef ROOM_H
#define ROOM_H

#include "definitions.h"
#include "history.h"

class client;

class room
{
	std::list<std::shared_ptr<client>> _participants;
	history _history_of_room;
public:
	room() {}
	room(const unsigned int& qd = 10);
	room(const room& r);
	void accept_client(client& new_client);
	void send_to_participants(const std::string& s, const std::string& name);
	void kick_user_out(const std::string& name);
	void kick_all_users_out();
	void get_participants(const std::string& name);
	void get_history(const std::string& name);
	void send_to(const SOCKET& sock, const std::string& message);
	~room() {}
};

#endif