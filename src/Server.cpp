#include "../include/Server.hpp"
#include <arpa/inet.h>

Server::Server(ServerConfig config_data) : _config_data(config_data){
	this->_Server_socket = 0;
	this->_len_Server_address = sizeof(_Server_address);
	// std::memset(&_Server_address, 0, sizeof(_Server_address));
	this->_Server_address.sin_family = AF_INET;
	if (inet_pton(AF_INET, config_data.getHost().c_str(), &this->_Server_address.sin_addr) <= 0)
	{
		std::cerr << RED << "Invalid address: " << config_data.getHost() << std::endl;
		exit(1);
	}
	this->_Server_address.sin_port = htons(static_cast<uint16_t>(std::stoi(config_data.getPort()))); // Port number
}

Server::~Server(){
}

int Server::startserver(){
	this->_Server_socket = socket(this->_Server_address.sin_family, SOCK_STREAM, 0);
	if (this->_Server_socket == -1)
	{
		std::cerr << RED << "Error creating socket" << std::endl;
		return 1;
	}
	if (bind(this->_Server_socket, (sockaddr *)&_Server_address, _len_Server_address) < 0)
	{
		std::cerr << RED << "Error binding" << std::endl;
		close (_Server_socket);
		return 1;
	}
	if (listen(_Server_socket, 128) < 0) //Linux default is 128
	{
		std::cout << RED << "Socket listen failed" << std::endl;
		close(_Server_socket);
		return 1;
	}
	uint32_t ip_host_order = ntohl(_Server_address.sin_addr.s_addr);
	
	std::cout << "\n*** Listening on ADDRESS: " 
	<< ((ip_host_order >> 24) & 0xFF) << "."
	<< ((ip_host_order >> 16) & 0xFF) << "."
	<< ((ip_host_order >> 8) & 0xFF) << "."
	<< (ip_host_order & 0xFF)
	
	<< " PORT: " << ntohs(this->_Server_address.sin_port) 
	<< " ***\n\n" << std::endl;
	return 0;
}

// int Server::start_accepting_connections(){
// 	std::cout << "====== Accepting Connections ======" << std::endl;
// 	while(true){
// 		//TODO check if epoll interest list is empty? - Not sure if this is needed. (Adrii has it, but he used poll not epoll)
// 		//EPOLL_WAIT:
// 		const int MAX_EVENTS = 10;
// 		struct epoll_event events[MAX_EVENTS];
// 		int epoll_num_events = epoll_wait(this->_epoll->get_epoll_fd(), events, MAX_EVENTS, -1); //TODO : check the parameters
// 		if (epoll_num_events == -1)
// 		{
// 			std::cerr << RED << "Error waiting for epoll events" << std::endl;
// 			return 1;
// 		}
// 		//PROCESSING EVENTS:
// 		for (int i = 0; i < epoll_num_events; ++i)
// 		{
// 			//If the socket fd is the server socket fd there is a new connection:
// 			if (events[i].data.fd == this->_Server_socket){
// 				int new_connection_socket_fd;
// 				struct sockaddr_in new_connection_address;
// 				socklen_t new_connection_address_len = sizeof(new_connection_address);
// 				new_connection_socket_fd = accept(this->_Server_socket, (struct sockaddr*)&new_connection_address, &new_connection_address_len);
// 				if (new_connection_socket_fd < 0)
// 				{
// 					std::cerr << RED << "Error accepting connection" << std::endl;
// 					return 1;
// 				}
// 				else
// 				{
// 					std::cout << "====== Processing Connection: " << new_connection_socket_fd << " ======" << std::endl;
// 					//ADD CLIENT TO EPOLL:
// 					if (this->_epoll->addEpollFd(new_connection_socket_fd == 1))
// 						return 1;
// 					//ADD CLIENT TO CLIENTS MAP:
// 					addClient(new_connection_socket_fd);
// 				}
// 			}
// 			//If the socket fd is a client socket fd, there is a request to read and a response to send:
// 			else if (events[i].events & EPOLLIN){
// 				std::cout << "====== Processing Client: " << events[i].data.fd << " ======" << std::endl;
// 				std::cout << "====== Processing Client Request ======" << std::endl;
// 				int bytes_received = 0;
// 				char buffer[BUFFER_SIZE] = {0}; //TODO: Fix this to parse the entire request, we are currently only reading a fixed BUFFER_SIZE
// 				bytes_received = recv(events[i].data.fd, buffer, BUFFER_SIZE, 0); //TODO: check Flags
// 				if (bytes_received < 0)
// 				{
// 					std::cout << RED << "Error receiving data from client" << std::endl;
// 					close(events[i].data.fd);
// 					//Remove client from epoll:
// 					epoll_ctl(this->_epoll->get_epoll_fd(), EPOLL_CTL_DEL, events[i].data.fd, NULL);
// 					//Remove client from clients map:
// 					_clients.erase(events[i].data.fd);
// 					std::cout << "Client disconnected: " << events[i].data.fd << std::endl;
// 					continue;
// 				}
// 				std::string request;
// 				request += buffer;
// 				std::cout << request << std::endl; //temporary, should be removed
// 				// _clients[events[i].data.fd].parseClientRequest(request);
// 				//Send response:
// 				//Clean up:
// 					//Close connection:
// 					//Remove client from epoll:
// 					//Remove client from clients map:
// 			}
// 		}
// 		return 0;
// 	}
// }

int Server::addClient(int new_connection_socket_fd){
	Client new_client(new_connection_socket_fd);
	auto insert_result = _clients.insert({new_connection_socket_fd, new_client});
	if (!insert_result.second) {
		std::cerr << RED << "Error adding client to clients map" << std::endl;
		return 1;
	}
	return 0;
}

int::Server::getServerSocket() const{
	return this->_Server_socket;
}