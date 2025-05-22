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

int Webserver::epoll_wait_util(){
	//EPOLL_WAIT:
	struct epoll_event events[this->_epoll_event_count];
	int epoll_num_ready_events = epoll_wait(this->_epoll_fd, events, this->_epoll_event_count, -1); //TODO : check the parameters
	if (epoll_num_ready_events == -1)
	{
		std::cerr << RED << "Error waiting for epoll events" << std::endl;
		return -1;
	}
	std::cout << GREEN << "Number of ready events: " << epoll_num_ready_events << std::endl;
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
	std::cout << "====== Accepting Connections ======" << std::endl;
	while(true){
		//TODO check if epoll interest list is empty? - Not sure if this is needed. (Adrii has it, but he used poll not epoll)
		//EPOLL_WAIT:
		int epoll_num_ready_events = epoll_wait_util();
		if (epoll_num_ready_events == -1)
			return 1;
		struct epoll_event events[this->_epoll_event_count];
		//PROCESSING EVENTS:
		for (int i = 0; i < epoll_num_ready_events; ++i)
		{
				//If the socket fd is the server socket fd there is a new connection:
				if (is_server_fd(events[i].data.fd)){
					if (start_accepting_connections(this->_servers[i]) == 1)
						return 1;
				}
				//If the socket fd is a client socket fd, there is a request to read and a response to send:
				else if (!is_server_fd(events[i].data.fd)){
					std::cout << "====== Processing Client: " << events[i].data.fd << " ======" << std::endl;
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

int Webserver::is_server_fd(int fd){
	for (size_t i = 0; i < this->_servers.size(); ++i){
		if (this->_servers[i].getServerSocket() == fd)
			return 0;
	}
	return 1;
}

int Webserver::process_request(int client_fd){
	int bytes_received = 0;
	char buffer[BUFFER_SIZE] = {0}; //TODO: Fix this to parse the entire request, we are currently only reading a fixed BUFFER_SIZE
	bytes_received = recv(client_fd, buffer, BUFFER_SIZE, 0); //TODO: check Flags
	std::cout << "Bytes received: " << bytes_received << std::endl;
	if (bytes_received < 0)
	{
		std::cout << RED << "Error receiving data from client" << std::endl;
		//Remove client from epoll:
		epoll_ctl(this->_epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
		//Remove client from clients map:
		// _servers.removeClient(client_fd);
		std::cout << "Client disconnected: " << client_fd << std::endl;
		return 1;
	}
	std::string request;
	request += buffer;
	std::cout << request << std::endl; //temporary, should be removed
	// _servers.getclient(client_fd).parseClientRequest(request);
	// _clients[events[i].data.fd].parseClientRequest(request);
	return 0;
}

int Webserver::start_accepting_connections(Server& server){
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
		server.addClient(new_connection_socket_fd); //TODO: Client is closing. Something related to out of scope Client instance?
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