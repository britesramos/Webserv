#include "../include/Server.hpp"
#include "../include/TcpServer.hpp"
#include "../include/ServerConfig.hpp"
#include "../include/ConfigParser.hpp"
#include "../include/Webserver.hpp"

void interrupt_helper(int sig)
{
	std::cout << "\n";
	std::cout << "\n	I was killed by the Ctrl+C\n" << std::endl;
	// TODO: close fds function with clean stuff, clean maps as well
	// server.closeserver();
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

		//1)Start/init the server(s) + epoll_instance:
		Webserver webserver;
		if (webserver.init_epoll() == 1)
		{
			std::cerr << "Failed to initialize epoll" << std::endl;
			return (1);
		}
		const std::vector<ServerConfig>& servers = file.getServer();
		// webserver.init_servers(servers);

		// // Debug: Print server FDs after initialization
		// webserver.printServerFDs();

		//2)Add server sockets to epoll interest list:
		if (webserver.addServerSockets() == 1)
		{
			//CLOSE FDS;
			return (1);
		}

		//3)Start accepting connections:
	

		//This still works (to be deleted once webserver class is working)
		TcpServer server = TcpServer(servers[0]);
		server.startListen();
	}
	else
	{
		std::cerr << "--- Incorrect amout of arguments ---" << std::endl;
		std::cerr << "   PROVIDE: ./webserv <file>.config" << std::endl;
		return (1);
	}
}
