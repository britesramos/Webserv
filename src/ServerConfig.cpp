#include "../include/ServerConfig.hpp"

ServerConfig::ServerConfig()
{
	this->host = "localhost";
	this->port = "8080";
	this->server_name = "";
	this->max_client_size = 1000000;
}

ServerConfig::~ServerConfig()
{
	std::cout << "Deleting construct." << std::endl;
}

void ServerConfig::setServerName(std::string input)
{
	this->server_name = input;
}

void ServerConfig::setHost(std::string input)
{
	this->host = input;
}

void ServerConfig::setPort(std::string input)
{
	this->server_name = input;
}

void ServerConfig::setMaxClientSize(int input)
{
	this->max_client_size = input;
}

void ServerConfig::setErrorPage(int error_number, std::string page)
{
	this->error_pages[error_number] = page;
}


std::string ServerConfig::getHost() const
{
	return (this->host);
}

std::string ServerConfig::getPort() const
{
	return (this->port);
}

std::string ServerConfig::getServerName() const
{
	return (this->server_name);
}
int ServerConfig::getMaxClientSize() const
{
	return(this->max_client_size);
}

std::string ServerConfig::getErrorPage(int code) const
{
	auto temp = this->error_pages.find(code);
	if (temp != this->error_pages.end()) // check if is not in the end of the map
		return (temp->second);
	else
		return ("");
}