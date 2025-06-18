#pragma once

#include "Server.hpp"
#include <vector>
#include <sstream>
#include <iostream>
#include <sys/epoll.h>
#include "../include/Cgi.hpp"
#include "../include/Location.hpp"
#include <fcntl.h>
#include <chrono> // For timeouts

#define SUCCESS 0
#define FAIL 1

#define TIMEOUT 3000 // miliseconds. it is good between 1 and 5 seconds
#define CLIENT_TIMEOUT 30 // seconds
#define CGI_TIMEOUT 10 // seconds

class Webserver{
	private:
		int _epoll_fd;
		int _epoll_event_count;
		std::vector<Server> _servers;
		std::unordered_map<int, int> client_server_map; // Map to store server-client relationships;
		// std::unordered_map<int, std::string> client_response_map; //or in each client. This way we dont need to cleand this map as well.
		std::map<int, std::shared_ptr<Client>> cgi_fd_to_client_map;        // For CGI output (read)
		std::map<int, std::shared_ptr<Client>> cgi_input_fd_to_client_map;  // For CGI input (write)

	public:
		Webserver();
		~Webserver();

		//Epoll methods
		int init_epoll();
		int addServerSockets();
		int addEpollFd(int new_connection_socket_fd, uint32_t events);
		int removeEpollFd(int socket_fd, uint32_t events);
		int modifyEpollEvent(int socket_fd, uint32_t events);
		int epoll_wait_util(struct epoll_event* events);

		//Server methods
		int init_servers(const std::vector<ServerConfig>& config_data_servers);
		int main_loop();
		int accept_connection(Server& server);
		bool is_server_fd(int fd);
		int process_request(int client_fd);
		int build_response(int client_fd);
		int send_response(int client_fd);

		void set_cgi_fd_to_client_map(int cgi_fd, std::shared_ptr<Client> client);
		void set_cgi_input_fd_to_client_map(int cgi_in_fd, std::shared_ptr<Client> client);

		//Handling Responses
		// int handle_cgi_request(int client_fd, const std::string& url_path);
		// int handle_error(std::shared_ptr<Client>& client);
		std::string build_header(std::string body);
		void close_connection(int connection_fd);

		//Debug methods
		void printServerFDs() const;

		//Getters
		const std::vector<Server>& get_servers() const;
		std::shared_ptr<Client>& getClientByClientFD(int client_fd);
		Server* getServerByClientFD(int client_fd);
		Server* getServerBySocketFD(int server_socket_fd);
		Location getLocationByPath(int client_fd, const std::string& url_path);

		int get_epoll_fd() const;
		void timeout_checks();

		std::shared_ptr<Client> get_client_by_cgi_fd(int cgi_fd);
		std::shared_ptr<Client> get_client_by_cgi_input_fd(int cgi_in_fd);
		bool processing_cgi(std::shared_ptr<Client>& client, int client_fd);
		//Clean_up_method
		void clean_up();
};