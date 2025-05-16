#include "../include/ServerConfig.hpp"

ServerConfig::ServerConfig()
{
	this->host = "localhost";
	this->port = "8080";
	this->server_name = "YarnServ";
	this->max_client_size = 1000000;
	// add default error_pages
}

ServerConfig::~ServerConfig()
{

}

// Setters
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
	this->port = input;
}

void ServerConfig::setMaxClientSize(int input)
{
	this->max_client_size = input;
}

void ServerConfig::setErrorPage(int error_number, std::string page)
{
	this->error_pages[error_number] = page;
}

void ServerConfig::addLocation(const Location& location)
{
	this->location_blocks.push_back(location);
}


// Getters
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

const std::unordered_map<std::string, Location>& ServerConfig::getLocations() const
{
	return (this->location_blocks);
}

void ServerConfig::print() const {
	std::cout << "Host: " << host << "\n";
	std::cout << "Port: " << port << "\n";
	std::cout << "Server_name: " << server_name << "\n";
	std::cout << "Max client size: " << max_client_size << "\n";

	std::cout << "Error Pages:\n";
	for (std::map<int, std::string>::const_iterator it = error_pages.begin(); it != error_pages.end(); ++it) {
		std::cout << "  " << it->first << " -> " << it->second << "\n";
	}

	std::cout << "Locations:\n";
	for (size_t i = 0; i < location_blocks.size(); ++i) {
		std::cout << "-- Location Block " << i << " --\n";
		location_blocks[i].print();
	}
}