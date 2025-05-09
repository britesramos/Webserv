#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include <iostream>
#include <map>
#include "../include/Location.hpp"

class ServerConfig{
	private:
		std::string host;
		std::string port;
		std::string server_name;
		int max_client_size;
		std::vector<Location> location_blocks;
		std::map<int, std::string> error_pages;

	public:
		ServerConfig();
		~ServerConfig();

	// Setters
		void setHost(std::string input);
		void setPort(std::string input);
		void setServerName(std::string input);
		void setMaxClientSize(int input);
		void setErrorPage(int error_number, std::string page);
		void addLocation(const Location& location);

	// Getters
		std::string getHost() const;
		std::string getPort() const;
		std::string getServerName() const;
		int getMaxClientSize() const;
		std::string getErrorPage(int code) const;
		const std::vector<Location>& getLocations() const;

		void print() const;
};
#endif