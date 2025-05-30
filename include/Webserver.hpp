#pragma once

#include "Server.hpp"
#include <vector>
#include <sstream>
#include <iostream>
#include <sys/epoll.h>
#include "../include/Cgi.hpp"
#include "../include/Location.hpp"

#define SUCCESS 0
#define FAIL 1

class Webserver{
	private:
		int _epoll_fd;
		int _epoll_event_count;
		std::vector<Server> _servers;
		std::unordered_map<int, int> client_server_map; // Map to store server-client relationships;
		// std::unordered_map<int, std::string> client_response_map; //or in each client. This way we dont need to cleand this map as well.
		Cgi cgi;
		//timer.

	public:
		Webserver();
		~Webserver();

		//Epoll methods
		int init_epoll();
		int addServerSockets();
		int addEpollFd(int new_connection_socket_fd, uint32_t events);
		int epoll_wait_util(struct epoll_event* events);

		//Server methods
		int init_servers(const std::vector<ServerConfig>& config_data_servers);
		int main_loop();
		int accept_connection(Server& server);
		bool is_server_fd(int fd);
		int process_request(int client_fd);
		int build_response(int client_fd);
		int send_response(int client_fd);

		//Handling Responses
		// int handle_cgi_request(int client_fd, const std::string& url_path);
		// int handle_get_request(std::shared_ptr<Client>& client, const std::string& url_path);
		// bool is_method_allowed(std::shared_ptr<Client>& client, const std::string& url_path, std::string method);
		// int handle_post_request(std::shared_ptr<Client>& client, const std::string& url_path);
		// int handle_delete_request(int client_fd, const std::string& url_path);
		// void handle_success(std::shared_ptr<Client>& client);
		// int handle_error(std::shared_ptr<Client>& client);
		// std::string build_status_line(std::shared_ptr<Client>& client, std::string status_code, std::string status_message);
		// std::string build_body(std::shared_ptr<Client>& client, const std::string& url_path, int flag);
		// std::string findRoot(int client_fd, const std::string& url_path);
		std::string build_header(std::string body);
		void close_connection(std::shared_ptr<Client>& client);
		//Handle Post Request Method
		// int handle_post_form_request(std::shared_ptr<Client>& client, const std::string& url_path);

		//Debug methods
		void printServerFDs() const;

		//Getters
		const std::vector<Server>& get_servers() const;
		std::shared_ptr<Client>& getClientByClientFD(int client_fd);
		Server* getServerByClientFD(int client_fd);
		Server* getServerBySocketFD(int server_socket_fd);
		Location getLocationByPath(int client_fd, const std::string& url_path);

		int get_epoll_fd() const;
};