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
	ConfigParser file;
	// function to handle Signal
	struct sigaction signalInterrupter;
	signalInterrupter.sa_handler = interrupt_helper;
	sigemptyset(&signalInterrupter.sa_mask);
	signalInterrupter.sa_flags = 0;
	sigaction(SIGINT, &signalInterrupter, 0);
	if (argc != 2)
	{
		std::cerr << "--- Incorrect amout of arguments ---" << std::endl;
		std::cerr << "   PROVIDE: ./webserv <file>.config" << std::endl;
		return (1);
	}
	if (file.is_file_extension_correct(argv[1]) == false)
		return (1);
	else
	{
		if (file.is_possible_use_file(argv[1]) == false)
			return (1);
	}
	if (file.is_server_config_load(file.getLines()) == false)
		return (1);


	const std::vector<ServerConfig>& servers = file.getServer();

	for (size_t i = 0; i < servers.size(); ++i) {
		std::cout << "=== Server " << i << " ===\n";
		servers[i].print();
	}
	TcpServer server = TcpServer(servers[0]);
	server.startListen();
	TcpServer server1 = TcpServer(servers[1]);
	server1.startListen();
}

// const std::vector<ServerConfig>& servers = file.getServer();

// for (size_t i = 0; i < servers.size(); ++i) {
	
// 	std::cout << "=== Server " << i << " ===\n";
// 	servers[i].print();
// 	tcp_servers[i] = TcpServer(servers[i]);
// }

// std::cout << "=== Listen 0 ===\n";
// servers[0].print();
// tcp_servers[0].startListen();

// for (size_t i = 0; i < servers.size(); ++i) {
	
// 	std::cout << "=== Server " << i << " ===\n";
// 	// servers[i].print();
// 	tcp_servers.push_back(TcpServer(servers[i]));
// }

// for (size_t i = 0; i < servers.size(); ++i) {
	
// 	std::cout << "=== Listen " << i << " ===\n";
// 	servers[i].print();
// 	tcp_servers[i].startListen();
// }