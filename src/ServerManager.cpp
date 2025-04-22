#include "../include/ServerManager.hpp"

void ServerManager::addServer(ServerConfig &server)
{
	servers.push_back(server);
}

const std::vector<ServerConfig>& ServerManager::getServer() const
{
	return (this->servers);
}