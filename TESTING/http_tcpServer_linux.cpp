#include "http_tcpServer_linux.h"

TcpServer::TcpServer(std::string ip_address, int port)
	: m_ip_address(ip_address), m_port(port), m_socket(),
	m_new_socket(), m_incomingMessage(), m_socketAddress(),
	m_socketAddress_len(sizeof(m_socketAddress)),
	m_serverMessage(buildResponse())
{
	startServer();
}

TcpServer::~TcpServer(){
	closeServer();
}

int TcpServer::startServer(){
	m_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket == -1){
		exitWithError("Cannot creat socket.");
		return 1;
	}
	else if (m_socket >= 0)
		std::cout << "m_socket: " << m_socket << std::endl;
	if (bind(m_socket, (sockaddr *)&m_socketAddress, m_socketAddress_len) < 0){
		exitWithError("Cannot connect socket to address");
	}
	return 0;
}

void TcpServer::startListen(){
	if (listen(m_socket, 20) < 0)
		exitWithError("Socket listen failed");
	std::ostringstream ss;
	ss << "\n*** Listening on ADDRESS: "
		<< inet_ntoa(m_socketAddress.sin_addr) //I dont fully understand this inet_ntoa function.
		<< " PORT: " << ntohs(m_socketAddress.sin_port)
		<< " ***\n\n";
	log(ss.str());
}

void TcpServer::closeServer(){
	close(m_socket);
	close(m_new_socket); //Do not understand this new_socket.
	exit (0);
}

void	log(const std::string &message){
	std::cout << message <<  std::endl;
}

void exitWithError(const std::string &errorMessage){
	log("ERROR " + errorMessage);
	exit (1);
}