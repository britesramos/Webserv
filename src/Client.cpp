#include "../include/Client.hpp"

Client::Client(int socket_fd):_Client_socket(socket_fd){
	std::cout << GREEN << "Client Request received -- " << this->_Client_socket << std::endl; //Should add some kind of client identifier (Socket FD?);
}

Client::~Client(){
	std::cout << "Client Request ---" << this->_Client_socket << "--- closed" << std::endl; //Should add some kind of client identifier (Socket FD?);
	if (this->_Client_socket != -1)
		close(this->_Client_socket);
}
//Finds the position of the delimiter in the string and returns it
int Client::getpos(std::string str, std::string delimiter, int start){
	size_t pos = str.find(delimiter, start);
	if (pos == std::string::npos)
	{
		std::cerr << RED << "Error on getpos function." << std::endl;
		return -1;
	}
	return pos;
}

//Parses the first line of the request
// Ex: GET /index.html HTTP/1.1
// Ex: POST /index.html HTTP/1.1
// Ex: DELETE /index.html HTTP/1.1
int Client::parse_firstline(std::string request){
	//Find the first line of the request:
	int pos = getpos(request, "\r\n", 0);
	if (pos == -1)
		return -1;
	std::string first_line = request.substr(0, pos);
	//Find the method, url_path and http_version:
	int pos1 = getpos(first_line, " ", 0);
	if (pos1 == -1)
		return -1;
	std::string method = first_line.substr(0, pos1);
	int pos2 = getpos(first_line, " ", pos1 + 1);
	if (pos2 == -1)
		return -1;
	std::string url_path = first_line.substr(pos1 + 1, pos2 - pos1 - 1);
	std::string http_version = first_line.substr(pos2 + 1);
	this->_Client_RequestMap.insert({"method", method});
	this->_Client_RequestMap.insert({"url_path", url_path});
	this->_Client_RequestMap.insert({"http_version", http_version});
	return 0;
}

int Client::parse_header(std::string request){
	//Find the start position of the header:
	int header_start = getpos(request, "\r\n", 0) + 2;
	if (header_start == -1)
	return -1;
	//Find the end position of the header:
	int header_end = getpos(request, "\r\n\r\n", 0);
	if (header_end == -1)
		return -1;
	//Get the header:
	std::string header = request.substr(header_start, header_end - header_start);
	// printf("Header: %s\n", header.c_str());
	//Split the header into lines + Loop through splited head and assign key (before ":") and value (after ":") to the map
	size_t pos = 0;
	pos = header.find("\r\n");
	while (pos != std::string::npos){
		std::string line = header.substr(0, pos);
		size_t delimiter_pos = line.find(":") + 1;
		if (delimiter_pos == std::string::npos){
			std::cout << RED << "Error parsing header line: " << line << std::endl;
			return -1;
		}
		std::string key = line.substr(0, delimiter_pos);
		std::string value = line.substr(delimiter_pos + 1);
		this->_Client_RequestMap.insert({key, value});
		header.erase(0, pos + 2);
		pos = header.find("\r\n");
	}
	return 0;
}

int Client::parse_body(std::string request){
	//Find the start position of the body:
	int body_start = getpos(request, "\r\n\r\n", 0) + 4;
	if (body_start == -1)
	return -1;
	//Get the body (everything after the header):
	std::string body = request.substr(body_start);
	this->_Client_RequestMap["body"] = body;
	// printf("body: %s\n", body.c_str()); //temp
	return 0;
}

int Client::parseClientRequest(std::string request){
	if (parse_firstline(request) < 0)
	{
		std::cerr << RED << "Error parsing first line of request" << std::endl;
		return -1;
	}
	if (parse_header(request) < 0)
	{
		std::cerr << RED << "Error parsing header of request" << std::endl;
		return -1;
	}
	//check chucked transfer encoding
	if (request.find("POST") != std::string::npos)
	{
		if (parse_body(request) < 0)
		{
			std::cerr << RED << "Error parsing body of request" << std::endl;
			return -1;
		}
	}
	return 0;
}


//***Getters***//
int Client::get_Client_socket(){
	return (this->_Client_socket);
}
const std::unordered_map<std::string, std::string>& Client::get_RequestMap() const{
	return (this->_Client_RequestMap);
}
const std::unordered_map<std::string, std::string>& Client::get_ResponseMap() const{
	return (this->_Client_ResponseMap);
}
std::string Client::get_Request(std::string key){
	return (this->_Client_RequestMap.at(key));
}
std::string Client::get_Response(std::string key){
	return (this->_Client_ResponseMap.at(key));
}

//***Setters***//
void Client::set_Client_socket(int socket_fd){
	this->_Client_socket = socket_fd;
}
void Client::set_Request(std::string key, std::string value){
	this->_Client_RequestMap[key] = value;
}
void Client::set_Response(std::string key, std::string value){
	this->_Client_ResponseMap[key] = value;
}

