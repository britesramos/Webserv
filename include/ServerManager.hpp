#ifndef SERVERMANAGER_HPP
#define SERVERMANAGER_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include "ServerConfig.hpp"

class ServerManager {
	private:
		std::vector<ServerConfig> servers;
	public:
		void addServer(ServerConfig &server);
		const std::vector<ServerConfig>& getServer() const;
};

#endif