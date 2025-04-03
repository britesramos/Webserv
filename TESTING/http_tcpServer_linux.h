#include <sys/socket.h>
#include <iomanip>
#include <iostream>
#include <unistd.h>

#pragma once

class TcpServer{
	public:
		TcpServer();
		~TcpServer();

		int	startServer();
		void closeServer();
		int	m_socket;
};