#include "../TcpServer.hpp"
// #include <vector>


unsigned long str_to_ulong(std::string str)
{
	unsigned long result;
	result = std::stoul(str); // it is just returning 127 before the dot, need to convert the roll number
	std::cout << "thid is the result " << result << std::endl;
	return result;
}

// std::vector<int> ip_segments;
// [127][0][0][1]

// u_long ip_bytes = ip_segments[0] << 24 | ip_segments[1] << 16 | ip_segments[2] << 8 | ip_segments[3];

TcpServer::TcpServer(std::string ip_address, int port) : m_socket(), m_new_socket(), m_socket_address()
{
	std::cout << "this is the construct" << std::endl;
	this->m_port = port;
	this->m_ipaddress = ip_address;
	this->m_len_socket_address = sizeof(m_socket_address);
	this->m_socket_address.sin_family = AF_INET;
	this->m_socket_address.sin_port = htons(port);
	this->m_socket_address.sin_addr.s_addr = htons(str_to_ulong(ip_address));
	startserver();
}

TcpServer::~TcpServer()
{
	closeserver();
}

int TcpServer::startserver()
{
	m_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket == -1)
	{
		std::cerr << "Error creating socket" << std::endl;
		return 1;
	}
	return 0;
}

void TcpServer::closeserver()
{
	std::cout << "this is the fd closed "<< m_socket << std::endl;
	close(m_socket);
	close(m_new_socket);
	exit(0);
}
