#include "../include/TcpServer.hpp"
#include "../include/Client.hpp"

// TODO Create a function to print cerr


static struct sockaddr_in init_socket_address(std::string ip_address, std::string port)
{
	struct sockaddr_in socket_address = {};
	struct addrinfo hints;
	struct addrinfo* result;

	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	// std::cout << port << std::endl;
	int status = getaddrinfo(ip_address.c_str() , port.c_str(), &hints, &result); // test: "!!!invalid_host$$$" for ip and abc123 for port
	if (status != 0 || result == nullptr) {
		std::cerr << "getaddrinfo failed: " << gai_strerror(status) << std::endl;
		exit (1);
	} else {
		std::memcpy(&socket_address, result->ai_addr, sizeof(struct sockaddr_in));
		// std::cout << "this is the value inside sockaddress: " << socket_address.sin_addr.s_addr << std::endl;
		// std::cout << "this is the value inside sockaddress port: " << socket_address.sin_port << std::endl;
		freeaddrinfo(result);
	}
	return socket_address;
}

TcpServer::TcpServer(ServerConfig config) : m_socket(), m_new_socket(), m_socket_address()
{
	// std::cout << "this is the construct" << std::endl;
	this->m_socket_address = init_socket_address(config.getHost(), config.getPort());
	this->m_len_socket_address = sizeof(m_socket_address);
	std::cout << "this is the value inside sockaddress out function: " << this->m_socket_address.sin_addr.s_addr << std::endl;
	if (startserver())
	{
		std::cerr << "Failed to start server" << std::endl;
		exit (1);
	}
}

TcpServer::~TcpServer()
{
	std::cout << "this is the fd closed "<< m_socket << std::endl;
	close(m_socket);
	close(m_new_socket);
	exit(0);
	// closeserver(); // is it necessary to create a close function?
}

TcpServer::TcpServer(const TcpServer &copy)
{
	this->m_len_socket_address = copy.m_len_socket_address;
	this->m_new_socket = copy.m_new_socket;
	this->m_socket = copy.m_socket;
	this->m_socket_address = copy.m_socket_address;
}

TcpServer& TcpServer::operator=(const TcpServer &copy)
{
	if(this != &copy)
	{
		this->m_len_socket_address = copy.m_len_socket_address;
		this->m_new_socket = copy.m_new_socket;
		this->m_socket = copy.m_socket;
		this->m_socket_address = copy.m_socket_address;
	}
	return *this;
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
		std::string request; //Used to parse the request, to send to parseClientRequest(request)
		acceptConnection(this->m_new_socket);
		Client client(this->m_new_socket); //Create client object to parse the client request
		
		char buffer[BUFFER_SIZE] = {0}; // TODO: Fix this to parse the entire request, we are currently only reading a fixed BUFFER_SIZE
		bytesReceived = read(this->m_new_socket, buffer, BUFFER_SIZE);
		if (bytesReceived < 0)
		{
			std::cerr << "Failed to read bytes from client socket connection" << std::endl;
			exit (1);
		}
		
		std::cout << "------ Received Request from client ------\n\n" << std::endl;
		std::cout << buffer << std::endl; //temporary, should be removed
		request = buffer;
		//Parsing client request:
		if (client.parseClientRequest(request) < 0)
			std::cout << "Error parsing client request" << std::endl;
		
		sendResponse();
		std::cout << "------ Client Request parsed ------\n\n" << std::endl;
		//Request map(printing):
		for (auto it = client.get_RequestMap().begin(); it != client.get_RequestMap().end(); ++it){
			std::cout << it->first << " => " << it->second << std::endl;
		}
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
//TO DO: rethink the sendResponse function, it shoudl handle the response for all the requests
void TcpServer::sendResponse()
{
	static int i = 0;
	i++;
	std::string htmlBody;
	if (i > 1) {
		std::ifstream file("yarncrochet.jpg"); // Open the yarncrochet.jpg file
		if (!file.is_open())
		{
			std::cerr << "Error: Could not open home_page.html" << std::endl;
			std::string errorBody = "<!DOCTYPE html><html lang=\"en\"><body><h1>500 Internal Server Error</h1><p>Could not load the requested file.</p></body></html>";
			std::string response = "HTTP/1.1 500 Internal Server Error\r\n";
			response += "Content-Type: text/html\r\n";
			response += "Content-Length: " + std::to_string(errorBody.size()) + "\r\n";
			response += "Connection: close\r\n";
			response += "\r\n";
			response += errorBody;
			write(m_new_socket, response.c_str(), response.size());
			return;
		}
		std::stringstream buffer;
		buffer << file.rdbuf();
		htmlBody = buffer.str();
		file.close();
	}
	else {
		std::ifstream file("home_page.html"); // Open the home_page.html file
		if (!file.is_open())
		{
			std::cerr << "Error: Could not open home_page.html" << std::endl;
			std::string errorBody = "<!DOCTYPE html><html lang=\"en\"><body><h1>500 Internal Server Error</h1><p>Could not load the requested file.</p></body></html>";
			std::string response = "HTTP/1.1 500 Internal Server Error\r\n";
			response += "Content-Type: text/html\r\n";
			response += "Content-Length: " + std::to_string(errorBody.size()) + "\r\n";
			response += "Connection: close\r\n";
			response += "\r\n";
			response += errorBody;
			write(m_new_socket, response.c_str(), response.size());
			return;
		}
		std::stringstream buffer;
		buffer << file.rdbuf();
		htmlBody = buffer.str();
		file.close();
	}
	std::string response = "HTTP/1.1 200 OK\r\n";
	if (i > 1) {
		response += "Content-Type: image/jpeg\r\n";
	}
	else {
		response += "Content-Type: text/html\r\n";
	}
	response += "Content-Length: " + std::to_string(htmlBody.size()) + "\r\n";
	response += "Connection: close\r\n";
	response += "\r\n";
	response += htmlBody;
	// Read the file contents into a string

	// Construct the HTTP response

	// Send the response
	unsigned long bytesSent = write(m_new_socket, response.c_str(), response.size());

	if (bytesSent == response.size())
	{
		std::cout << "------ home_page.html served as landing page ------\n\n" << std::endl;
	}
	else
	{
		std::cerr << "Error sending response to client" << std::endl;
	}
}



// void TcpServer::closeserver()
// {
// 	std::cout << "this is the fd closed "<< m_socket << std::endl;
// 	close(m_socket);
// 	close(m_new_socket);
// 	exit(0);
// }
