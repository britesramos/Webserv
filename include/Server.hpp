#pragma once

#include "ConfigParser.hpp"
#include "Epoll.hpp"
#include <netinet/in.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <string.h>
#include "Client.hpp"

#define BUFFER_SIZE 1024

class Server{
	private:
		int _Server_socket;
		// Epoll *_epoll; //take out
		struct sockaddr_in _Server_address;
		unsigned int _len_Server_address;
		ServerConfig _config_data;
		std::unordered_map<int, Client> _clients;

	public:
		Server(ServerConfig config_data);
		~Server();
		int startserver();
		int init_epoll();
		int start_accepting_connections();
		int addClient(int new_connection_socket_fd);

		//Getters
		int getServerSocket() const;
		
};