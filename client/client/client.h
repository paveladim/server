#ifndef CLIENT_H
#define CLIENT_H

#include "definitions.h"

class client
{
	WSAData _wsa_data;
	SOCKET _client_socket;
	bool _is_connected;
public:
	client();
	std::string receive();
	void send_to(const std::string& message);
	void handler();
	void sender();
	~client();
};

#endif 
