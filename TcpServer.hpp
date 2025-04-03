#ifndef TCPSERVER_HPP
#define TCPSERVER_HPP

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h> // sockaddr
#include <unistd.h>


class TcpServer{
	private:
		int m_socket;
		int m_new_socket;
		struct sockaddr_in m_socket_address;
		int m_len_socket_address;
		int m_port;
		std::string m_ipaddress; // create a converter to net byte Htonl or Hton

		int startserver();
		void closeserver();
	public:
		TcpServer(std::string ip_address, int port); // will received from the config file
		~TcpServer();
};
#endif