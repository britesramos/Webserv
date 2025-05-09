#pragma once

#include "ConfigParser.hpp"
#include <netinet/in.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <string.h>
#include "Client.hpp"

#define BUFFER_SIZE 1024

class Server{
	private:
		int _Server_socket;
		struct sockaddr_in _Server_address;
		unsigned int _len_Server_address;
		ServerConfig _config_data;
		std::unordered_map<int, Client> _clients;

	public:
		Server(ServerConfig config_data);
		~Server();

		int startserver();
		int init_epoll();
		int addClient(int new_connection_socket_fd);
		int removeClient(int client_fd);

		//Getters
		int getServerSocket() const;
		Client& getclient(int client_fd);
		
};