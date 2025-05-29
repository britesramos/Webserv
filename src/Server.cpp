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

int Server::addClient(int new_connection_socket_fd){
	auto new_client = std::make_shared<Client>(new_connection_socket_fd, this->_config_data);
	auto insert_result = _clients.insert({new_connection_socket_fd, new_client});
	if (!insert_result.second) {
		std::cerr << RED << "Error adding client to clients map" << std::endl;
		return 1;
	}
	std::cout << GREEN << "Client added to clients map: " << new_connection_socket_fd << std::endl;
	return 0;
}

int Server::removeClient(int client_fd){
	auto it = _clients.find(client_fd);
	if (it != _clients.end()) {
		_clients.erase(it);
		std::cout << GREEN << "Client removed from clients map: " << client_fd << std::endl;
		return 0;
	}
	std::cerr << RED << "Error removing client from clients map" << std::endl;
	return 1;
}

//Getters
std::shared_ptr<Client>& Server::getclient(int client_fd){
	auto it = _clients.find(client_fd);
	if (it != _clients.end()) {
		return it->second;
	}
	throw std::runtime_error("Client not found");
}

ServerConfig Server::getServerConfig() const{
	return this->_config_data;
}

const std::unordered_map<int, std::shared_ptr<Client>>& Server::getClients() const{
	return this->_clients;
}

int::Server::getServerSocket() const{
	return this->_Server_socket;
}

ServerConfig Server::getconfigData() const{
	return (this->_config_data);
}

//Utils

void Server::printClientRequests() const{
	for (const auto& client_pair : _clients) {
		std::cout << "Client FD: " << client_pair.first << std::endl;
		const auto& client = client_pair.second;
		const auto& request_map = client->get_RequestMap();
		for (const auto& request_pair : request_map) {
			std::cout << request_pair.first << ": " << request_pair.second << std::endl;
		}
	}
}