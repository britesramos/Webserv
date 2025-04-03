#include "http_tcpServer_linux.h"

TcpServer::TcpServer(){

}

TcpServer::~TcpServer(){

}

int TcpServer::startServer(){
	m_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket == -1){
		std::cout << "Cannot creat socket." << std::endl;
		return 1;
	}
	else if (m_socket >= 0)
		std::cout << "m_socket: " << m_socket << std::endl;
	return 0;
}

void TcpServer::closeServer(){
	close(m_socket);
	exit (0);
}