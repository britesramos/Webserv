#include "../include/Server.hpp"
#include "../include/TcpServer.hpp"
#include "../include/ServerConfig.hpp"
#include "../include/ConfigParser.hpp"

void interrupt_helper(int sig)
{
	std::cout << "\n";
	std::cout << "\n	I was killed by the Ctrl+C\n" << std::endl;
	// TODO close fds function with clean stuff
	exit(sig + 128);
}

int main(int argc, char **argv)
{
	//---------------------------------------------------------------------------------//
	// function to handle Signal - Ctrl - CAN WE CLEAN THIS UP? MAKE SIGNALS THEIR OWN THING?
	struct sigaction signalInterrupter;
	signalInterrupter.sa_handler = interrupt_helper;
	sigemptyset(&signalInterrupter.sa_mask);
	signalInterrupter.sa_flags = 0;
	sigaction(SIGINT, &signalInterrupter, 0);
	//---------------------------------------------------------------------------------//
	if (argc == 2)
	{
		ConfigParser file;
		if (file.config_file_parsing(argv[1]) == false)
			return (1);
		//-----------------------------------TO BE DELETED---------------------------------//
		// for (size_t i = 0; i < servers.size(); ++i) {
		// 	std::cout << "=== Server " << i << " ===\n";
		// 	servers[i].print();
		// }
		//---------------------------------------------------------------------------------//

		//1)Start the server(s):
		Epoll epoll;
		if (epoll.init_epoll() == 1)
		{
			std::cerr << "Failed to initialize epoll" << std::endl;
			return (1);
		}
		const std::vector<ServerConfig>& servers = file.getServer();
		//Create vector of server objects: (depends on number of servers in config file)
		int number_of_servers = file.getNumberOfServers();
		std::vector<Server> webservers(number_of_servers);
		//TODO: The following loop should start each server with the correct config data from parsing.
		//I need to iterate over both vectors.
		for (const auto& server : servers)
		{
			Server webservers(server);
			if (webservers.startserver() == 1)
			{
				std::cerr << "Failed to start server." << std::endl;
				return (1);
			}
			epoll.addServerSocket(webservers.getServerSocket());
		}
		// epoll.start_accepting_connections();

		//This still works (to be deleted once server class is working)
		// TcpServer server = TcpServer(servers[0]);
		// server.startListen();
	}
	else
	{
		std::cerr << "--- Incorrect amout of arguments ---" << std::endl;
		std::cerr << "   PROVIDE: ./webserv <file>.config" << std::endl;
		return (1);
	}
}
