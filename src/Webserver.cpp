#include "../include/Webserver.hpp"
#include <iostream>
#include <sys/epoll.h>

//Constructor and Destructor
Webserver::Webserver(){
	this->_epoll_fd = 0;
}
Webserver::~Webserver(){
	close(_epoll_fd);
}

//Epoll methods
int Webserver::init_epoll(){
	this->_epoll_fd = epoll_create1(0);
	if (this->_epoll_fd == -1)
		return 1;
	return 0;
}

int Webserver::addServerSockets(){
	struct epoll_event event;
	for (size_t i = 0; i < this->_servers.size(); ++i){
		int server_fd = this->_servers[i].getServerSocket();
		if (server_fd <= 0){
			std::cout << RED << "Error: Invalid server socket fd: " << server_fd << std::endl;
			return 1;
		}
		event.data.fd = this->_servers[i].getServerSocket();
		event.events = EPOLLIN;
		if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_ADD, this->_servers[i].getServerSocket(), &event) < 0){
			std::cout << RED << "Error adding server socket fd to epoll interest list: " << event.data.fd << std::endl;
			return 1;
		}
		this->_epoll_event_count++;
		std::cout << GREEN << "Added server socket fd to epoll interest list: " << event.data.fd << std::endl;
	}
	return 0;
}
int Webserver::addEpollFd(int new_connection_socket_fd){
	if (new_connection_socket_fd <= 0){
		std::cout << RED << "Error: Invalid socket fd: " << new_connection_socket_fd << std::endl;
		return 1;
	}
	struct epoll_event event;
	event.data.fd = new_connection_socket_fd;
	event.events = EPOLLIN | EPOLLET; // Edge Triggered ??? Not sure if this is what we want. This option is harder to implement apparently. Other option is level triggered.
	if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_ADD, new_connection_socket_fd, &event) < 0){
		std::cout << RED << "Error adding socket fd to epoll interest list: " << event.data.fd << std::endl;
		return 1;
	}
	this->_epoll_event_count++;
	std::cout << GREEN << "Added new socket fd to epoll interest list: " << event.data.fd << std::endl;
	return 0;
}

int Webserver::epoll_wait_util(struct epoll_event* events){
	//EPOLL_WAIT:
	int epoll_num_ready_events = epoll_wait(this->_epoll_fd, events, this->_epoll_event_count, -1); //TODO : check the parameters
	if (epoll_num_ready_events == -1)
	{
		std::cerr << RED << "Error waiting for epoll events" << std::endl;
		return -1;
	}
	std::cout << GREEN << "				Number of ready events: " << epoll_num_ready_events << std::endl;
	return epoll_num_ready_events;
}

//Server methods
int Webserver::init_servers(const std::vector<ServerConfig>& config_data_servers){
	for (size_t i = 0; i < config_data_servers.size(); ++i) {
		Server temp(config_data_servers[i]);
		if (temp.startserver() == 1)
		{
			std::cerr << "Failed to start server." << std::endl;
			return (1);
		}
		this->_servers.push_back(temp);
	}
	return 0;
}

int Webserver::main_loop(){
	std::cout << "\n====== !!! WEBSERV PARTY TIME !!! ======\n" << std::endl;
	while(true){
		//TODO check if epoll interest list is empty? - Not sure if this is needed. (Adrii has it, but he used poll not epoll)
		//EPOLL_WAIT:
		std::cout << "\n\n=============== !!! EPOLL_WAIT TIME !!! ===============" << std::endl;
		struct epoll_event events[this->_epoll_event_count];
		int epoll_num_ready_events = epoll_wait_util(events);
		if (epoll_num_ready_events == -1)
			return 1;
		//PROCESSING EVENTS:
		for (int i = 0; i < epoll_num_ready_events; ++i)
		{
			std::cout << "Event fd: " << events[i].data.fd << std::endl; //Temporary, should be removed
				//If the socket fd is the server socket fd there is a new connection:
				if (is_server_fd(events[i].data.fd) == true){
					if (accept_connection(this->_servers[i]) == 1)
						return 1;
				}
				//If the socket fd is a client socket fd, there is a request to read and a response to send:
				else {
					std::cout << "Processing client fd: " << events[i].data.fd << std::endl; //Temporary, should be removed
					if (process_request(events[i].data.fd) == 1)
						return 1;
					
					// send_response(events[i].data.fd);
					// 	//Clean up:
					// 		//Close connection:
					// 		//Remove client from epoll:
					// 		//Remove client from clients map:
				}
			}
		}
		return 0;
}
	
bool Webserver::is_server_fd(int fd){
	for (size_t i = 0; i < this->_servers.size(); ++i){
		if (this->_servers[i].getServerSocket() == fd){
			std::cout << GREEN << "Socket fd is a server socket fd: " << fd << std::endl;
			return true;
		}
	}
	std::cout << RED << "Error: Socket fd is not a server socket fd: " << fd << std::endl;
	return false;
}

int Webserver::process_request(int client_fd){
	std::cout << GREEN << "\n====== Processing Client: " << client_fd << " ======" << std::endl;
	int bytes_received = 0;
	char buffer[BUFFER_SIZE] = {0}; //TODO: Fix this to parse the entire request, we are currently only reading a fixed BUFFER_SIZE
	bytes_received = recv(client_fd, buffer, BUFFER_SIZE, 0); //TODO: check Flags
	std::cout << "Bytes received: " << bytes_received << std::endl;
	if (bytes_received <= 0)
	{
		if (bytes_received == 0)
			std::cout << "Client disconnected: " << client_fd << std::endl;
		else{
			std::cout << RED << "Error receiving data from client: " << client_fd << " - " << strerror(errno) << std::endl;
		}
		//Remove client from epoll:
		epoll_ctl(this->_epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
		//Remove client from clients map:
		for (size_t i = 0; i < this->_servers.size(); ++i){
			if (this->_servers[i].removeClient(client_fd) == 0){
				std::cout << GREEN << "Client removed from clients map: " << client_fd << std::endl;
				break;
			}
		}
		return 1;
	}
	std::string request;
	request += buffer;
	std::cout << "REQUEST: " << request << std::endl; //temporary, should be removed

	//THIS IS NOT WORKING - SEGFAULT:
	// int this_client_server = this->client_server_map.find(client_fd)->second;
	// std::cout << "this_client_server: " << this_client_server << std::endl;
	// std::shared_ptr<Client> client = this->_servers[this_client_server].getclient(client_fd);
	// client->parseClientRequest(request);
	return 0;
}

int Webserver::accept_connection(Server& server){
	int new_connection_socket_fd;
	struct sockaddr_in new_connection_address;
	socklen_t new_connection_address_len = sizeof(new_connection_address);
	new_connection_socket_fd = accept(server.getServerSocket(), (struct sockaddr*)&new_connection_address, &new_connection_address_len);
	// std::cout << GREEN << "New connection accepted: " << new_connection_socket_fd << std::endl;
	if (new_connection_socket_fd < 0)
	{
		std::cerr << RED << "Error accepting connection" << std::endl;
		return 1;
	}
	else
	{
		std::cout << "====== Processing Connection: " << new_connection_socket_fd << " ======" << std::endl;
		//ADD CLIENT TO EPOLL INTEREST LIST:
		if (this->addEpollFd(new_connection_socket_fd) == 1){
			std::cerr << RED << "Error adding new connection socket fd to epoll interest list" << std::endl;
			return 1;
		}
		//ADD CLIENT TO SERVER CLIENTS MAP:
		server.addClient(new_connection_socket_fd);
		//ADD CLIENT TO SERVER CLIENTS MAP:
		this->client_server_map.insert({new_connection_socket_fd, server.getServerSocket()});
	}
	std::cout << "Current Clients in Server:" << std::endl;
    for (const auto& client_pair : server.getClients()) { // Assuming getClients() returns the unordered_map
        std::cout << "Client FD: " << client_pair.first << std::endl;
    }
	return 0;
}

//Getters and Setters
const std::vector<Server>& Webserver::get_servers() const{
	return this->_servers;
}
int Webserver::get_epoll_fd() const{
	return this->_epoll_fd;
}

//Utility methods
void Webserver::printServerFDs() const {
	std::cout << "Server FDs:" << std::endl;
	for (size_t i = 0; i < _servers.size(); ++i) {
		std::cout << "Server " << i << " FD: " << _servers[i].getServerSocket() << std::endl;
	}
}
//THIS WILL NEVER WORK REALLY:
// void Webserver::printEpollInterestList() const {
// 	std::cout << "Epoll Interest List:" << std::endl;
// 	struct epoll_event events[10]; // Assuming a maximum of 10 events for demonstration
// 	int event_count = epoll_wait(_epoll_fd, events, 10, 0);
// 	for (int i = 0; i < event_count; ++i) {
// 		std::cout << "FD: " << events[i].data.fd << std::endl;
// 	}
// }