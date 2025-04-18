#include "../include/TcpServer.hpp"
#include "../include/ServerConfig.hpp"

void interrupt_helper(int sig)
{
	std::cout << "\n";
	std::cout << "\n	I was killed by the Ctrl+C\n" << std::endl;
	// TODO close fds function with clean stuff
	exit(sig + 128);
}

static bool is_possible_use_file(std::string input)
{
	std::ifstream infile(input);
	if (infile.is_open())
	{
		if (infile.peek() == EOF)
		{
			std::cerr << "Your config file is empty." << std::endl;
			infile.close();
			return false;
		}
		infile.close();
		return true;
	}
	else
	{
		std::cerr << "Are you sure the file \"" << input << "\" can be open?" << std::endl;
		return false;
	}
}

static bool is_file_extension_correct(std::string input)
{
	if (input.substr(input.find_last_of(".") + 1) == "conf")
	{
		return (true);
	}
	else
	{
		std::cerr << "Please, provide \".conf\" file" << std::endl;
		return (false);
	}
}

int main(int argc, char **argv)
{
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
	if (is_file_extension_correct(argv[1]) == false)
		return (1);
	else
	{
		//check if it can be open and if it is not empty
		if (is_possible_use_file(argv[1]) == false)
			return (1);
	}
	//Configuration file parsing HERE.
	ServerConfig config;

	config.setHost("127.0.0.1");
    config.setPort("8080");
    config.setServerName("example.com");
    config.setMaxClientSize(5000000);
    config.setErrorPage(404, "/error_pages/404.html");
    config.setErrorPage(500, "/error_pages/500.html");
	std::cout << "host: " << config.getHost() << std::endl;
	std::cout << "port: " << config.getPort() << std::endl;
	std::cout << "server name: " << config.getServerName() << std::endl;
	std::cout << "max client size: " << config.getMaxClientSize() << std::endl;
	std::cout << "error page 4-4: " << config.getErrorPage(404) << std::endl;
	std::cout << "error page 500: " << config.getErrorPage(500) << std::endl;
	if (config.getErrorPage(100) == "") // doesnt have this number page on the map
		std::cout << "empty bitch" << std::endl;
	// TcpServer server = TcpServer("localhost", "8080"); //This values need to be replaced by configuration file parsed data.
	// server.startListen();
}