#pragma once

#include "ConfigParser.hpp"
#include <netinet/in.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <string.h>
#include "Client.hpp"
#include <memory>

#define BUFFER_SIZE 1024

class Server{
	private:
		int _Server_socket;
		struct sockaddr_in _Server_address;
		unsigned int _len_Server_address;
		ServerConfig _config_data;
		std::unordered_map<int, std::shared_ptr<Client>> _clients;

	public:
		Server(ServerConfig config_data);
		~Server();

		int startserver();
		int init_epoll();
		int addClient(int new_connection_socket_fd);
		int removeClient(int client_fd);

		//Getters
		int getServerSocket() const;
		ServerConfig getServerConfig() const;
		std::shared_ptr<Client>& getclient(int client_fd);
		const std::unordered_map<int, std::shared_ptr<Client>>& getClients() const;
		std::unordered_map<int, std::shared_ptr<Client>>& getClients(); // for timeout porposes
		ServerConfig getconfigData() const;
		
		//Utils
		void printClientRequests() const;
};