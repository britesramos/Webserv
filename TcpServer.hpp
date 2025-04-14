#ifndef TCPSERVER_HPP
#define TCPSERVER_HPP

#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h> // sockaddr
#include <sys/types.h>
#include <unistd.h>
#include <cstring>
#include <netdb.h>
#include <signal.h>
#include <iostream>
#include <fstream>



#define BUFFER_SIZE 4096

class TcpServer{
	private:
		int m_socket;
		int m_new_socket;
		struct sockaddr_in m_socket_address;
		unsigned int m_len_socket_address;

		int startserver();
		void acceptConnection(int &new_socket);
		// void closeserver();
		void sendResponse();
		public:
		TcpServer(std::string ip_address, std::string port); // will received from the config file
		~TcpServer();

		void startListen();
};

#endif