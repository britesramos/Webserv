#pragma once

#include "ConfigParser.hpp"
#include <netinet/in.h>

class Server{
	private:
		int _Server_socket;
		int _Client_socket;
		struct sockaddr_in _Server_address;
		unsigned int _len_Server_address;
		std::vector<ServerConfig> _servers;

	public:
		Server();
		~Server();
		int startserver();
		int startlisten();

		void setServer(std::vector<ServerConfig> servers);
		
};