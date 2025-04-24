#include "../include/ClientRequest.hpp"

ClientRequest::ClientRequest()
{
	std::cout << "Client Request received" << std::endl; //Should add some kind of client identifier (Socket FD?);
}

ClientRequest::~ClientRequest()
{
	//TO DO
	//Close the socket
	std::cout << "Client Request closed" << std::endl; //Should add some kind of client identifier (Socket FD?);
}