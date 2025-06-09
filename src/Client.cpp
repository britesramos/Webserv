#include "../include/Client.hpp"
#include "../include/Cgi.hpp"

Client::Client(int socket_fd, ServerConfig& server_config):_Client_socket(socket_fd), _server_config(server_config){
	this->_error_code = "200";
	this->isCGI = false;
	std::cout << GREEN << "Client Request received -- " << this->_Client_socket << std::endl; //Should add some kind of client identifier (Socket FD?);
}

Client::~Client(){
	std::cout << "Client Request ---" << this->_Client_socket << "--- closed" << std::endl; //Should add some kind of client identifier (Socket FD?);
	// if (this->_Client_socket != -1)
	// 	close(this->_Client_socket);
	// delete this->cgi;
}

// Client::Client(const Client& other){
// 	this->_Client_socket = other._Client_socket;
// 	this->_error_code = other._error_code;
// 	this->_Client_RequestMap = other._Client_RequestMap;
// 	this->_response = other._response;
// 	this->_server_config = other._server_config;
// }

Client& Client::operator=(const Client& other){
	if (this != &other)
	{
		this->_Client_socket = other._Client_socket;
		this->_error_code = other._error_code;
		this->_request_buffer = other._request_buffer;
		this->_Client_RequestMap = other._Client_RequestMap;
		this->_response = other._response;
		this->_server_config = other._server_config;
		this->isCGI = other.isCGI;
	}
	return *this;
}

//Parsing methods:
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
int Client::parse_firstline(){
	//Find the first line of the request:
	int pos = getpos(this->_request_buffer, "\r\n", 0);
	if (pos == -1)
		return -1;
	std::string first_line = this->_request_buffer.substr(0, pos);
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

int Client::parse_header(){
	//Find the start position of the header:
	int header_start = getpos(this->_request_buffer, "\r\n", 0) + 2;
	if (header_start == -1)
	return -1;
	//Find the end position of the header:
	int header_end = getpos(this->_request_buffer, "\r\n\r\n", 0);
	if (header_end == -1)
		return -1;
	//Get the header:
	std::string header = this->_request_buffer.substr(header_start, header_end - header_start);
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

int Client::parse_body(){
	//Find the start position of the body:
	int body_start = getpos(this->_request_buffer, "\r\n\r\n", 0) + 4;
	if (body_start == -1)
	return -1;
	//Get the body (everything after the header):
	std::string body = this->_request_buffer.substr(body_start);
	this->_Client_RequestMap["body"] = body;
	// printf("body: %s\n", body.c_str()); //temp
	return 0;
}

int Client::parseClientRequest(){
	if (parse_firstline() < 0)
	{
		std::cerr << RED << "Error parsing first line of request" << std::endl;
		return -1;
	}
	if (parse_header() < 0)
	{
		std::cerr << RED << "Error parsing header of request" << std::endl;
		return -1;
	}
	if (this->_request_buffer.find("POST") != std::string::npos)
	{
		if (parse_body() < 0)
		{
			std::cerr << RED << "Error parsing body of request" << std::endl;
			return -1;
		}
	}
	return 0;
}

//Building response methods:

int Client::handle_get_request(){
	std::string response;
	std::string url_path = this->get_Request("url_path");
	if (is_method_allowed(url_path, "GET") == true){
		//1)Build response:
		std::cout << "GET request for: " << url_path << std::endl;
		std::string body = build_body(url_path, 0);
		if (body.empty())
			return 1;
		std::string header = build_header(body);
		std::string status_line = build_status_line("200", "OK");
		response = status_line + header;
		std::cout << "Response: " << response << std::endl;
		response += body;
	}
	this->_response = response;
	return 0;
}

int Client::handle_post_request(){
	std::string response;
	std::string url_path = this->get_Request("url_path");
	std::cout << "URL_PATH:::: " << url_path << std::endl;
	if (is_method_allowed(url_path, "POST") == true)
		handle_success();
	return 0;
}

int Client::handle_cgi_response(Cgi& cgi)
{
	std::cout << "		Handling CGI response..." << std::endl;
	char buffer[1024];
	ssize_t bytes_read;
	
	bytes_read = read(cgi.get_cgi_out(READ), buffer, sizeof(buffer) - 1);
	if (bytes_read < 0) {
		perror("read");
		return -1;
	}
	else if (bytes_read == 0) {
		std::cout << "CGI process has finished reading output." << std::endl;
		std::string status_line = build_status_line("200", "OK");
		std::string header = build_header(cgi_buffer);
		std::string full_response = status_line + header + cgi_buffer;

		std::cout << "Final CGI Response:\n" << full_response << std::endl;
		this->_response = full_response;

		// check for error in python! trash trash trash
		if (cgi_buffer.find("Traceback") != std::string::npos) {
			size_t trace_pos = cgi_buffer.find("Traceback");
			std::string traceback = cgi_buffer.substr(trace_pos);
			std::cerr << "CGI Error:\n" << traceback << std::endl;
			this->set_error_code("500");
			return -1;
		}
		this->cgi_buffer.clear();
		return 1;
	}
	else {
		buffer[bytes_read] = '\0';
		cgi_buffer += buffer;
		return 0;
	}
}

std::string Client::build_status_line (std::string status_code, std::string status_message){
	std::string http_version = this->_Client_RequestMap["http_version"];
	std::string status_line = http_version + " " + status_code + " " + status_message + "\r\n";
	return (status_line);
}

std::string Client::build_header(std::string body){
	std::string header = "Content-Type: text/html\r\n";
	header += "Content-Length: " + std::to_string(body.size()) + "\r\n";
	header += "Connection: close\r\n";
	header += "\r\n";
	return (header);
}

//TODO: I think it is only matching url_paths to the locations map, it should look for the logest prefix match instead.
std::string Client::findRoot(const std::string& url_path){
	std::string root;
	std::unordered_map<std::string, Location> locations;
	locations = this->_server_config.getLocations();
	
	std::string best_match = "";
	for (const auto& pair : locations) {
		const std::string& path = pair.first;
		if (url_path.compare(0, path.length(), path) == 0 && 
		    (url_path.length() == path.length() || url_path[path.length()] == '/' || path == "/")) {
			if (path.length() > best_match.length())
				best_match = path;
		}
	}

	const Location& location = locations.at(best_match);
	//TODO: Find a way to check if there is a location that matches that url_path.
	//1) If no location found, return default root:
	// else{
	// 	std::cerr << RED << "Error: No location found for url path: " << url_path << std::endl;
	// 	root = "www/";
	// }
	//2) Get root from location:
	root = location.getRoot();
	if (root.empty()){
		std::cerr << RED << "Error: No root found for url path: " << url_path << std::endl;
		return "www/";
	}
	//3) Return root:
	return root;
}

std::string Client::build_body(const std::string& url_path, int flag){
	std::cout << "Building response body for: " << url_path << std::endl;
	//1)Find file to build body with:
	std::string root;
	if (flag == 1)
		root = url_path;
	else{
		if (url_path == "/")
			root = "www/html/.html";
		else{
			root = findRoot(url_path);
			root += url_path;
		}
	}
	std::cout << "Root in build_body after findRoot : " << root << std::endl;
	//2)Opening file and converting into a string to be send back at the client:
	std::ifstream html_file(root);
	std::ostringstream body_stream;
	if (!html_file.is_open()){
		set_error_code("404");
		std::string body = body_stream.str();
		return body;
	}
	body_stream << html_file.rdbuf();
	std::string body = body_stream.str();
	html_file.close();
	return body;
}

bool Client::is_method_allowed(const std::string& url_path, std::string method){
	std::unordered_map<std::string, Location> locations = this->_server_config.getLocations();

	std::string best_match = "";
	for (const auto& pair : locations) {
		const std::string& path = pair.first;
		if (url_path.compare(0, path.length(), path) == 0 && 
		    (url_path.length() == path.length() || url_path[path.length()] == '/' || path == "/")) {
			if (path.length() > best_match.length())
				best_match = path;
		}
	}

	const Location& location = locations.at(best_match);
	// Location location = locations[url_path];
	const std::vector<std::string> allowed_methods = location.get_methods();
	for (size_t i = 0; i < allowed_methods.size(); ++i){
		std::cout << "method: " << allowed_methods[i] << std::endl; //TEMP
		if (allowed_methods[i] == method)
			return true;
	}
	std::cout << RED << "METHOD NOT ALLOWED!" << std::endl; //What happens in this case? The website doesnt change?
	this->_error_code = "405";
	return false;
}

void Client::handle_success(){
	std::string body = build_body("www/html/Success.html", 1);
	std::string header = build_header(body);
	std::string status_line = build_status_line("200", "OK");
	std::string response = status_line + header;
	response += body;
	std::cout << "SUCCESS RESPONSE: " << response << std::endl;
	this->_response = response;
}

void Client::handle_error(){
	std::cout << RED << "Error: " << this->_error_code << std::endl;
	std::string error_message = _server_config.getErrorPage(this->_error_code);
	std::string status_line = build_status_line(this->_error_code, error_message);
	std::string body = build_body("error_pages/" + this->_error_code + ".html", 1);
	std::string header = build_header(body);
	std::string response = status_line + header;
	std::cout << "Response: " << response << std::endl;
	response += body;
	this->_response = response;
}


//***Getters***//
int Client::get_Client_socket(){
	return (this->_Client_socket);
}
const std::unordered_map<std::string, std::string>& Client::get_RequestMap() const{
	return (this->_Client_RequestMap);
}
std::string Client::get_Request(std::string key){
	std::string request;
	try {
		request = this->_Client_RequestMap.at(key);
	} catch (...)
	{
		request = "";
	}
	return (request);
}
std::string Client::get_Response(){
	return (this->_response);
}
std::string Client::get_error_code(){
	return (this->_error_code);
}

bool Client::get_isCgi()
{
	return (this->isCGI);
}

int Client::get_cgiOutputfd()
{
	return this->CgiOutputfd;
}

Cgi* Client::get_cgi()
{
	return this->cgi;
}

std::string Client::get_requestBuffer(){
	return this->_request_buffer;
}

//***Setters***//
void Client::set_Client_socket(int socket_fd){
	this->_Client_socket = socket_fd;
}
void Client::set_Request(std::string key, std::string value){
	this->_Client_RequestMap[key] = value;
}
void Client::set_error_code(std::string error_code){
	this->_error_code = error_code;
}

void Client::set_isCgi(bool value)
{
	this->isCGI = value;
}

void Client::set_cgiOutputfd(int fd)
{
	this->CgiOutputfd = fd;
}

void Client::set_cgi(Cgi* cgi)
{
	this->cgi = cgi;
}

void Client::appendToBufferRequest(std::string to_append){
	this->_request_buffer += to_append;
}

void Client::clearBuffer(){
	this->_request_buffer = "";
}
