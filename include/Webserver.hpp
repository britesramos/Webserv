#pragma once

#include "Server.hpp"
#include <vector>
#include <sstream>
#include <iostream>
#include <sys/epoll.h>
#include "../include/Cgi.hpp"
#include "../include/Location.hpp"

class Webserver{
	private:
		int _epoll_fd;
		int _epoll_event_count;
		std::vector<Server> _servers;
		std::unordered_map<int, int> client_server_map; // Map to store server-client relationships;
		Cgi cgi;

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
		int send_response(int client_fd);

		//Handling Responses
		// int handle_cgi_request(int client_fd, const std::string& url_path);
		int handle_get_request(int client_fd, const std::string& url_path);
		int handle_post_request(int client_fd, const std::string& url_path);
		// int handle_delete_request(int client_fd, const std::string& url_path);
		void handle_success(int client_fd);
		int handle_error(int client_fd, int error_code, std::string error_message);
		std::string build_response_body(const std::string& url_path);
		std::string findRoot(int client_fd, const std::string& url_path);
		std::string build_header(std::string body);

		//Debug methods
		void printServerFDs() const;

		//Getters
		const std::vector<Server>& get_servers() const;
		Server* getServerBySocketFD(int server_socket_fd);
		Location getLocationByPath(int client_fd, const std::string& url_path);

		int get_epoll_fd() const;
};