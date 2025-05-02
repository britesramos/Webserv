#include "../include/Server.hpp"

Server::Server(){
}

Server::~Server(){
}

void Server::setServer(std::vector<ServerConfig> servers)
{
	this->_servers = servers;
}

int Server::startserver(){
	this->_Server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_Server_socket == -1)
	{
		std::cerr << "Error creating socket" << std::endl;
		return 1;
	}
	if (bind(this->_Server_socket, (sockaddr *)&_Server_address, _len_Server_address) < 0)
	{
		std::cerr << "Error binding" << std::endl;
		return 1;
	}
	return 0;
}

int Server::startlisten(){
	if (listen(_Server_socket, 20) < 0) //Why the maximum backlog is 20?
	{
		std::cout << "Socket listen failed" << std::endl;
		exit (1);
	}
	return 0;
}