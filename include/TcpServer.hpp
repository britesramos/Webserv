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
#include <fstream>
#include "ServerConfig.hpp"
#include <sstream>
#include "Cgi.hpp"


class TcpServer{
	private:
		int m_socket;
		int m_new_socket;
		struct sockaddr_in m_socket_address;
		unsigned int m_len_socket_address;
		Cgi cgi;

		int startserver();
		void acceptConnection(int &new_socket);
		void sendResponse();
		public:
		TcpServer();
		TcpServer(ServerConfig config); // will received from the config file
		// ~TcpServer();
		
		TcpServer(const TcpServer &copy);
		TcpServer& operator=(const TcpServer &copy);
		
		void startListen();
		
		
		bool is_cgi(std::string response);
		void return_forbidden();
		void closeserver();
};

#endif