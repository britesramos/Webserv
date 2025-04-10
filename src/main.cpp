// #include <iostream>

// // int main (int argc, char **argv)
// // {
// // 	if (argc > 2)
// // 	{
// // 		std::cout << "Wrong Amount of Arguments" << std::endl;
// // 		return 1;
// // 	}
// // 	else
// // 	{
// // 		if(argc == 2)
// // 		{
// // 			std::cout << argv[1] << std::endl;
// // 			//config file should be used as argv[1]
// // 		}
// // 		else
// // 		{
// // 			std::cout << "Default"<< std::endl;
// // 			// should use a default config file
// // 		}
// // 	}
// // }

// /*
// 1. Create a socket with function socket() (IPv4 and TCP).
// 2. Bind the socket with a port 8080 with bind().
// 3. You make the socket listen() --> this will make it a "server socket".
// 4. 

// */

// /*
// 	struct epoll_event something[1000];
// 	epoll_wait()
// 	epoll_event something[0] = data.fd
// 	epoll_event something[1] = data.fd
// */

// /*handle client(int fd) {
// 	//what you are sending needs to be in the same protocol
// 	//<html><h1>Hello World!!</h1></html>
// 	send(client_fd, what)
// }
// */

// int main() {
// 	//create the server socket
// 	//create the 1 (only one!) epoll();
// 	while(true) {
// 		//number of ready events = epoll_wait() --> return the number of "ready fd's".
// 		//for loop that loops through the "ready fd's" --> epoll
// 		//if (data.fd == server.fd)
// 		// accept the client with accept();
// 		// add it to the epoll instance with epoll_ctl(client_fd)
// 		//else
// 		// int client_fd = event.data.fd
// 		//	handle_client(client_fd)
// 	}
// }


// #include <iostream>
// #include <sys/epoll.h>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <unistd.h>
// #include <cstring>

// #define PORT 8080
// #define MAX_EVENTS 10

// void handle_client(int client_fd) {
// 	char buffer[1024];
// 	read(client_fd, buffer, sizeof(buffer));
// 	std::cout << buffer << std::endl;
//     const char response[] = "HTTP/1.1 200 Ok\r\nContent-Type: text/html\r\n\r\n<html><h1>HELLO WORLD!</h1></html>";
//     send(client_fd, response, sizeof(response), 0);
//     close(client_fd);
// }

// int main() {
//     int server_fd = socket(AF_INET, SOCK_STREAM, 0); // tcp socket
// 	std::cout << "Socket created succesfully "  << server_fd << std::endl;
//     if (server_fd == -1) {
//         std::cerr << "Failed to create socket" << std::endl;
//         return 1;
//     }

// 	// attach socket to the port
//     sockaddr_in server_addr{};
//     server_addr.sin_family = AF_INET;
//     server_addr.sin_addr.s_addr = INADDR_ANY;
//     server_addr.sin_port = htons(PORT);

//     if (bind(server_fd, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
//         std::cerr << "Bind error " << strerror(errno) << std::endl;
//         return 1;
//     }
// 	std::cout << "Bind Successful on port " << PORT << std::endl;

//     if (listen(server_fd, SOMAXCONN) < 0) {
//         std::cerr << "Listen failed" << std::endl;
//         return 1;
//     }

//     int epoll_fd = epoll_create(MAX_EVENTS);
//     if (epoll_fd == -1) {
//         std::cerr << "Failed to create epoll" << std::endl;
//         return 1;
//     }

//     epoll_event event{};
//     event.events = EPOLLIN;
//     event.data.fd = server_fd;
//     if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event))
// 	{
// 		std::cerr << "Failed to add file descriptor to epoll" << std::endl;
// 		if (close(epoll_fd)){
// 			std::cerr << "Failed to close epoll file descriptor" << std::endl;
// 			return 1;
// 		}
// 		return 1;
// 	}

//     epoll_event events[MAX_EVENTS];

//     while (true) {
//         int event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
//         for (int i = 0; i < event_count; ++i) {
//             if (events[i].data.fd == server_fd) {
//                 sockaddr_in client_addr;
//                 socklen_t client_len = sizeof(client_addr);
//                 int client_fd = accept(server_fd, (sockaddr*)&client_addr, &client_len);
//                 if (client_fd >= 0) {
//                     epoll_event client_event{};
//                     client_event.events = EPOLLIN;
//                     client_event.data.fd = client_fd;
//                     epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &client_event);
//                 }
//             } else {
//                 handle_client(events[i].data.fd);
//             }
//         }
//     }

//     if(close(server_fd))
// 	{
// 		std::cerr << "Failed to close server file descriptor" << std::endl;
// 		return 1;
// 	}
//     if(close(epoll_fd))
// 	{
// 		std::cerr << "Failed to close epoll file descriptor" << std::endl;
// 		return 1;
// 	}
//     return 0;
// }


#include "../TcpServer.hpp"


void interrupt_helper(int sig)
{
	std::cout << '\n';
	std::cout << "\n	I was killed by the Ctrl+C\n" << std::endl;
	// TODO close fds function with clean stuff
	exit(sig + 128);
}

static bool is_file_extension_correct(std::string input)
{
	if (input.substr(input.find_last_of(".") + 1) == "conf")
	{
		return (true);
	}
	else
	{
		std::cout << "Please, provide \".conf\" file" << std::endl;
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
		std::cout << "	Incorrect amout of arguments" << std::endl;
		std::cout << "	 Provide: ./webserv [file].config" << std::endl;
		return (1);
	}
	if (is_file_extension_correct(argv[1]) == false)
		return (1);
	else
	{
		//check if it can be open and if it is not empty
	}
	TcpServer server = TcpServer("localhost", "8080");
	server.startListen();
}