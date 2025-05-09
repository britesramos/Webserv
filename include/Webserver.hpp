#pragma once

#include "Server.hpp"
#include <vector>

class Webserver{
	private:
		int _epoll_fd;
		int _epoll_event_count;
		std::vector<Server> _servers;

	public:
		Webserver();
		~Webserver();

		//Epoll methods
		int init_epoll();
		int addServerSockets();
		int addEpollFd(int new_connection_socket_fd);
		int epoll_wait_util();

		//Server methods
		int init_servers(const std::vector<ServerConfig>& config_data_servers);
		int main_loop();
		int start_accepting_connections(Server server);

		//Debug methods
		void printServerFDs() const;

		//Getters
		const std::vector<Server>& get_servers() const;
		int get_epoll_fd() const;
};