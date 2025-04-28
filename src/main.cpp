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
	// const std::vector<std::string>& lines = file.getLines();
    // for (size_t i = 0; i < lines.size(); ++i) {
    //     std::cout << "Line " << i << ": " << lines[i] << std::endl;
    // }

	// std::vector<std::string> allTokens;

	// for (const std::string& line : file.getLines()) {
	// 	std::vector<std::string> lineTokens = file.tokenize_line(line);
	// 	allTokens.insert(allTokens.end(), lineTokens.begin(), lineTokens.end()); // add to full token list
	// }

	// for (size_t i = 0; i < allTokens.size(); ++i) {
    //     std::cout << "Tokens " << i << ": " << allTokens[i] << "#!"<< std::endl;
    // }
	// ServerConfig config;

	// config.setHost("127.0.0.1");
    // config.setPort("8080");
    // config.setServerName("example.com");
    // config.setMaxClientSize(5000000);
    // config.setErrorPage(404, "/error_pages/404.html");
    // config.setErrorPage(500, "/error_pages/500.html");
	// std::cout << "host: " << file.getHost() << std::endl;
	// std::cout << "port: " << config.getPort() << std::endl;
	// std::cout << "server name: " << config.getServerName() << std::endl;
	// std::cout << "max client size: " << config.getMaxClientSize() << std::endl;
	// std::cout << "error page 4-4: " << config.getErrorPage(404) << std::endl;
	// std::cout << "error page 500: " << config.getErrorPage(500) << std::endl;

	// const std::vector<ConfigParser>& temp = file.getServer();
	// for (size_t i = 0; i < temp.size(); ++i) {
	// 	std::cout << "Server block " << i << ":\n";
	// 	for (const auto& pair : temp[i]) {
	// 		std::cout << "  " << pair.first << " = " << pair.second << std::endl;
	// 	}
	// 	std::cout << std::endl;
	// file.getServer();
	// if (config.getErrorPage(100) == "") // doesnt have this number page on the map
	// 	std::cout << "empty bitch" << std::endl;
	// TcpServer server = TcpServer("localhost", "8080"); //This values need to be replaced by configuration file parsed data.
	// server.startListen();
}