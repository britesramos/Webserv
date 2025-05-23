#include "../include/ServerConfig.hpp"

ServerConfig::ServerConfig()
{
	this->host = "localhost";
	this->port = "8080";
	this->server_name = "YarnServ";
	this->max_client_size = 1000000;
	this->error_pages["400"] = "/error_pages/400.html";
	this->error_pages["403"] = "/error_pages/403.html";
	this->error_pages["404"] = "/error_pages/404.html";
	this->error_pages["413"] = "/error_pages/413.html";
	this->error_pages["500"] = "/error_pages/500.html";
	this->error_pages["502"] = "/error_pages/502.html";
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

void ServerConfig::setErrorPage(std::string error_number, std::string page)
{
	this->error_pages[error_number] = page;
}

void ServerConfig::addLocation(std::string path, Location& location)
{
	this->location_blocks.insert(std::make_pair(path, location));
	// this->location_blocks.push_back(location);
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

std::string ServerConfig::getErrorPage(std::string code) const
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
	for (std::map<std::string, std::string>::const_iterator it = error_pages.begin(); it != error_pages.end(); ++it) {
		std::cout << "  " << it->first << " -> " << it->second << "\n";
	}

	std::cout << "Locations:\n";
	for (std::unordered_map<std::string, Location>::const_iterator it = location_blocks.begin(); it != location_blocks.end(); ++it) {
		std::cout << "  " << it->first << " -> ";
		it->second.print();
		std::cout << "\n";
	}

}