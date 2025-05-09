#pragma once

#include "Server.hpp"
#include <vector>

class Webserver{
	private:
		int _epoll_fd;
		std::vector<Server> _servers;

	public:
		Webserver();
		~Webserver();

		//Epoll methods
		int init_epoll();
		int addServerSockets();
		int addEpollFd(int new_connection_socket_fd);

		//Server methods
		int init_servers(const std::vector<ServerConfig>& config_data_servers);

		//Debug methods
		void printServerFDs() const;
		void printEpollInterestList() const;

		//Getters
		const std::vector<Server>& get_servers() const;
		int get_epoll_fd() const;
};