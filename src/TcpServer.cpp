#include "../TcpServer.hpp"

// TODO Create a function to print cerr


static struct sockaddr_in init_socket_address(std::string ip_address, std::string port)
{
	struct sockaddr_in socket_address = {};
	struct addrinfo hints;
	struct addrinfo* result;

	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	int status = getaddrinfo(ip_address.c_str(), port.c_str(), &hints, &result); // test: "!!!invalid_host$$$" for ip and abc123 for port
	if (status != 0 || result == nullptr) {
		std::cerr << "getaddrinfo failed: " << gai_strerror(status) << std::endl;
	} else {
		std::memcpy(&socket_address, result->ai_addr, sizeof(struct sockaddr_in));
		std::cout << "this is the value inside sockaddress: " << socket_address.sin_addr.s_addr << std::endl;
		std::cout << "this is the value inside sockaddress port: " << socket_address.sin_port << std::endl;
		std::cout << "this is the value inside status: " << status << std::endl;
		freeaddrinfo(result);
	}
	return socket_address;
}

TcpServer::TcpServer(std::string ip_address, std::string port) : m_socket(), m_new_socket(), m_socket_address()
{
	std::cout << "this is the construct" << std::endl;
	this->m_socket_address = init_socket_address(ip_address, port);
	this->m_len_socket_address = sizeof(m_socket_address);
	std::cout << "this is the value inside sockaddress out function: " << this->m_socket_address.sin_addr.s_addr << std::endl;
	if (startserver())
	{
		std::cerr << "========== Failed to start server ==========" << std::endl;
		exit (1);
	}
}

TcpServer::~TcpServer()
{
	closeserver();
}

int TcpServer::startserver()
{
	this->m_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (this->m_socket == -1)
	{
		std::cerr << "Error creating socket" << std::endl;
		return 1;
	}
	if (bind(this->m_socket, (sockaddr *)&m_socket_address, m_len_socket_address) < 0)
	{
		std::cerr << "Error binding" << std::endl;
		return 1;
	}
	return 0;
}

void TcpServer::startListen()
{
	if (listen(m_socket, 20) < 0)
	{
		std::cout << "Socket listen failed" << std::endl;
		exit (1);
	}

	uint32_t ip_host_order = ntohl(m_socket_address.sin_addr.s_addr);

	std::cout << "\n*** Listening on ADDRESS: " 
		<< ((ip_host_order >> 24) & 0xFF) << "."
			<< ((ip_host_order >> 16) & 0xFF) << "."
			<< ((ip_host_order >> 8) & 0xFF) << "."
			<< (ip_host_order & 0xFF)
	
		<< " PORT: " << ntohs(this->m_socket_address.sin_port) 
		<< " ***\n\n" << std::endl;

	int bytesReceived;

	while (true)
	{
		std::cout << "====== Waiting for a new connection ======\n\n\n" << std::endl;
		acceptConnection(this->m_new_socket);

		char buffer[BUFFER_SIZE] = {0};
		bytesReceived = read(this->m_new_socket, buffer, BUFFER_SIZE);
		if (bytesReceived < 0)
		{
			std::cerr << "Failed to read bytes from client socket connection" << std::endl;
			exit (1);
		}

		std::cout << "------ Received Request from client ------\n\n" << std::endl;

		sendResponse();

		// close(m_new_socket);
	}
}

void TcpServer::acceptConnection(int &new_socket)
{
	std::cout << "====== Connection Accepted =====" << std::endl;
	new_socket = accept(this->m_socket, (sockaddr *)&this->m_socket_address, &this->m_len_socket_address);
	if (new_socket < 0)
	{
		std::cout << "Server failed to accept incoming connection from ADDRESS: " << ntohs(m_socket_address.sin_addr.s_addr) << "; PORT: " << ntohs(this->m_socket_address.sin_port);
		exit (1);
	}
}

void TcpServer::sendResponse()
{
	std::string htmlBody = "<!DOCTYPE html><html lang=\"en\"><body><h1> HOME </h1><p> Hello from your Server :) </p></body></html>";
    
    std::string response = "HTTP/1.1 200 OK\r\n";
    response += "Content-Type: text/html\r\n";
    response += "Content-Length: " + std::to_string(htmlBody.size()) + "\r\n";
    response += "Connection: close\r\n";
    response += "\r\n";
    response += htmlBody;

	unsigned long bytesSent;

	bytesSent = write(m_new_socket, response.c_str(), response.size());

	if (bytesSent == response.size())
	{
		std::cout << "------ Server Response sent to client ------\n\n" << std::endl;
	}
	else
	{
		std::cout << "Error sending response to client" << std::endl;
	}
}

void TcpServer::closeserver()
{
	std::cout << "this is the fd closed "<< m_socket << std::endl;
	close(m_socket);
	close(m_new_socket);
	exit(0);
}
