#include "../include/Server.hpp"
#include "../include/ServerConfig.hpp"
#include "../include/ConfigParser.hpp"
#include "../include/Webserver.hpp"

bool g_stop = 0;

void interrupt_helper(int sig)
{
    (void)sig;
	std::cout << "\n";
	std::cout << "\n	I was killed by the Ctrl+C\n" << std::endl;
    g_stop = 1;
}

int main(int argc, char **argv)
{
	//---------------------------------------------------------------------------------//
	// function to handle Signal - Ctrl
	struct sigaction signalInterrupter;
	signalInterrupter.sa_handler = interrupt_helper;
	sigemptyset(&signalInterrupter.sa_mask);
	signalInterrupter.sa_flags = 0;
	sigaction(SIGINT, &signalInterrupter, 0);
	//---------------------------------------------------------------------------------//
	if (argc <= 2)
	{
		ConfigParser file;
		std::string input;
		if (argc == 1)
			input = "./config_files/config_2.conf";
		else
			input = argv[1];

		if (file.config_file_parsing(input) == false)
			return (1);
		
		//1)Start/init the server(s) + epoll_instance:
		Webserver webserver;
		if (webserver.init_epoll() == 1)
		{
			std::cerr << "Failed to initialize epoll" << std::endl;
            file.clean_up();
			return (1);
		}
		const std::vector<ServerConfig>& servers = file.getServer();
		if (webserver.init_servers(servers) == 1)
		{
			std::cerr << "Failed to initialize servers" << std::endl;
            file.clean_up();
			return (1);
		}

		// //2)Add server sockets to epoll interest list:
		if (webserver.addServerSockets() == 1)
		{
			webserver.clean_up();
            file.clean_up();
			return (1);
		}
		// //3)Start accepting connections:
		if (webserver.main_loop() == 1 || g_stop)
		{
			webserver.clean_up();
            file.clean_up();
			return (1);
		}
	}
	else
	{
		std::cerr << RED << "--- Incorrect amout of arguments ---" << std::endl;
		std::cerr << RED << "   PROVIDE: none or only ONE config file" << std::endl;
		return (1);
	}
}
