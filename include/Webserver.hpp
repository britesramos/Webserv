#pragma once

#include "Server.hpp"
#include <vector>

class Webserver{
	private:
		int _epoll_fd;
		int _epoll_event_count;
		std::vector<Server> _servers;
		std::unordered_map<int, int> client_server_map; // Map to store server-client relationships;

	public:
		Webserver();
		~Webserver();

		//Epoll methods
		int init_epoll();
		int addServerSockets();
		int addEpollFd(int new_connection_socket_fd);
		int epoll_wait_util(struct epoll_event* events);

		//Server methods
		int init_servers(const std::vector<ServerConfig>& config_data_servers);
		int main_loop();
		int accept_connection(Server& server);
		bool is_server_fd(int fd);
		int process_request(int client_fd);
		// int send_response(int client_fd);

		//Debug methods
		void printServerFDs() const;

		//Getters
		const std::vector<Server>& get_servers() const;

		int get_epoll_fd() const;
};