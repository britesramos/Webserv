#include "../include/Webserver.hpp"

//Constructor and Destructor
Webserver::Webserver(){
	this->_epoll_fd = 0;
}
Webserver::~Webserver(){
	close(_epoll_fd);
}

//Epoll methods
int Webserver::init_epoll(){
	this->_epoll_fd = epoll_create1(0);
	if (this->_epoll_fd == -1)
		return 1;
	return 0;
}

int Webserver::addServerSockets(){
	struct epoll_event event;
	for (size_t i = 0; i < this->_servers.size(); ++i){
		int server_fd = this->_servers[i].getServerSocket();
		if (server_fd <= 0){
			std::cout << RED << "Error: Invalid server socket fd: " << server_fd << std::endl;
			return 1;
		}
		event.data.fd = this->_servers[i].getServerSocket();
		event.events = EPOLLIN;
		if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_ADD, this->_servers[i].getServerSocket(), &event) < 0){
			std::cout << RED << "Error adding server socket fd to epoll interest list: " << event.data.fd << std::endl;
			return 1;
		}
		this->_epoll_event_count++;
		std::cout << GREEN << "Added server socket fd to epoll interest list: " << event.data.fd << std::endl;
	}
	return 0;
}
int Webserver::addEpollFd(int new_connection_socket_fd){
	if (new_connection_socket_fd <= 0){
		std::cout << RED << "Error: Invalid socket fd: " << new_connection_socket_fd << std::endl;
		return 1;
	}
	struct epoll_event event;
	event.data.fd = new_connection_socket_fd;
	event.events = EPOLLIN | EPOLLET; // Edge Triggered ??? Not sure if this is what we want. This option is harder to implement apparently. Other option is level triggered.
	if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_ADD, new_connection_socket_fd, &event) < 0){
		std::cout << RED << "Error adding socket fd to epoll interest list: " << event.data.fd << std::endl;
		return 1;
	}
	this->_epoll_event_count++;
	std::cout << GREEN << "Added new socket fd to epoll interest list: " << event.data.fd << std::endl;
	return 0;
}

int Webserver::epoll_wait_util(struct epoll_event* events){
	//EPOLL_WAIT:
	int epoll_num_ready_events = epoll_wait(this->_epoll_fd, events, this->_epoll_event_count, -1); //TODO : check the parameters
	if (epoll_num_ready_events == -1)
	{
		std::cerr << RED << "Error waiting for epoll events" << std::endl;
		return -1;
	}
	std::cout << GREEN << "				Number of ready events: " << epoll_num_ready_events << std::endl;
	return epoll_num_ready_events;
}

//Server methods
int Webserver::init_servers(const std::vector<ServerConfig>& config_data_servers){
	for (size_t i = 0; i < config_data_servers.size(); ++i) {
		Server temp(config_data_servers[i]);
		if (temp.startserver() == 1)
		{
			std::cerr << "Failed to start server." << std::endl;
			return (1);
		}
		this->_servers.push_back(temp);
	}
	return 0;
}

int Webserver::main_loop(){
	std::cout << "\n====== !!! WEBSERV PARTY TIME !!! ======\n" << std::endl;
	while(true){
		//TODO check if epoll interest list is empty? - Not sure if this is needed. (Adrii has it, but he used poll not epoll)
		//EPOLL_WAIT:
		std::cout << GREEN << "\n\n=============== !!! EPOLL_WAIT TIME !!! ===============" << std::endl;
		struct epoll_event events[this->_epoll_event_count];
		int epoll_num_ready_events = epoll_wait_util(events);
		if (epoll_num_ready_events == -1)
			return 1;
		//PROCESSING EVENTS:
		for (int i = 0; i < epoll_num_ready_events; ++i)
		{
			std::cout << "Event fd: " << events[i].data.fd << std::endl; //Temporary, should be removed
				//If the socket fd is the server socket fd there is a new connection:
				if (is_server_fd(events[i].data.fd) == true){
					if (accept_connection(this->_servers[i]) == 1)
						return 1;
				}
				//If the socket fd is a client socket fd, there is a request to read and a response to send:
				else {
					std::cout << "Processing client fd: " << events[i].data.fd << std::endl; //Temporary, should be removed
					if (process_request(events[i].data.fd) == -1){
						std::cout << "HOLLA MUCHACHOS!" << std::endl;
						break ;
					}

					//Print client requests for all servers (TO BE DELETED OR COMMENTED OUT):
					// std::cout << "\n\n================= !!! CLIENT REQUESTS !!! =================" << std::endl;
					// for (size_t j = 0; j < this->_servers.size(); ++j){
					// 	std::cout << "Server: " << j <<  "Client Requests Map: " << std::endl;
					// 	this->_servers[j].printClientRequests();
					// }


					if (send_response(events[i].data.fd) == 1)
						return 1;
					// 	//Clean up:
					// 		//Close connection:
					// 		//Remove client from epoll:
					// 		//Remove client from clients map:
				}
			}
		}
		return 0;
}

int Webserver::send_response(int client_fd){
	Server* server = getServerBySocketFD(this->client_server_map.find(client_fd)->second);
	std::shared_ptr<Client>& client = server->getclient(client_fd);
	std::string response;
	// if (client->get_Request("url_path").find("/cgi-bin/") != std::string::npos) //Passar isto para dentro de cada method.
	// {
	// 	std::cout << "CGI response" << std::endl;
	// 	cgi.run_cgi(*client);
	// }
	if (client->get_Request("method") == "GET")
		handle_get_request(client_fd, client->get_Request("url_path"));
	else if (client->get_Request("method") == "POST")
		handle_post_request(client_fd, client->get_Request("url_path"));
	// if (client->get_Request("method") == "DELETE")
	// 	handle_delete_request(client_fd, client->get_Request("url_path"));
	else
		handle_error(client_fd, 400, "Bad Request");
	return 0;
}

bool Webserver::is_server_fd(int fd){
	for (size_t i = 0; i < this->_servers.size(); ++i){
		if (this->_servers[i].getServerSocket() == fd){
			std::cout << GREEN << "Socket fd is a server socket fd: " << fd << std::endl;
			return true;
		}
	}
	std::cout << RED << "Socket fd is not a server socket fd: " << fd << std::endl;
	return false;
}

int Webserver::process_request(int client_fd){
	std::cout << GREEN << "\n====== Processing Client: " << client_fd << " ======" << std::endl;
	int bytes_received = 0;
	char buffer[BUFFER_SIZE] = {0}; //TODO: Fix this to parse the entire request, we are currently only reading a fixed BUFFER_SIZE
	bytes_received = recv(client_fd, buffer, BUFFER_SIZE, 0); //TODO: check Flags
	std::cout << "Bytes received: " << bytes_received << std::endl;
	if (bytes_received <= 0)
	{
		if (bytes_received == 0)
			std::cout << "Client disconnected: " << client_fd << std::endl;
		else{
			std::cout << RED << "Error receiving data from client: " << client_fd << " - " << strerror(errno) << std::endl;
		}
		//Remove client from epoll:
		epoll_ctl(this->_epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
		//Remove client from clients map:
		for (size_t i = 0; i < this->_servers.size(); ++i){
			if (this->_servers[i].removeClient(client_fd) == 0){
				return -1;
			}
		}
		return 1;
	}
	std::string request;
	request += buffer;
	std::cout << "REQUEST: " << request << std::endl; //temporary, should be removed
	//Parse client request into currect client object:
	int this_client_server = this->client_server_map.find(client_fd)->second;
	std::cout << "this_client_server: " << this_client_server << std::endl;
	std::shared_ptr<Client>& client = getServerBySocketFD(this_client_server)->getclient(client_fd);
	client->parseClientRequest(request);
	return 0;
}

int Webserver::accept_connection(Server& server){
	int new_connection_socket_fd;
	struct sockaddr_in new_connection_address;
	socklen_t new_connection_address_len = sizeof(new_connection_address);
	new_connection_socket_fd = accept(server.getServerSocket(), (struct sockaddr*)&new_connection_address, &new_connection_address_len);
	// std::cout << GREEN << "New connection accepted: " << new_connection_socket_fd << std::endl;
	if (new_connection_socket_fd < 0)
	{
		std::cerr << RED << "Error accepting connection" << std::endl;
		return 1;
	}
	else
	{
		std::cout << "====== Processing Connection: " << new_connection_socket_fd << " ======" << std::endl;
		//ADD CLIENT TO EPOLL INTEREST LIST:
		if (this->addEpollFd(new_connection_socket_fd) == 1){
			std::cerr << RED << "Error adding new connection socket fd to epoll interest list" << std::endl;
			return 1;
		}
		//ADD CLIENT TO SERVER CLIENTS MAP:
		server.addClient(new_connection_socket_fd);
		//ADD CLIENT TO SERVER CLIENTS MAP:
		this->client_server_map.insert({new_connection_socket_fd, server.getServerSocket()});
	}
	std::cout << "Current Clients in Server:" << std::endl;
    for (const auto& client_pair : server.getClients()) { // Assuming getClients() returns the unordered_map
        std::cout << "Client FD: " << client_pair.first << std::endl;
    }
	return 0;
}

//Handling Responses
std::string Webserver::build_response_body(const std::string& url_path){
	std::cout << "Building response body for: " << url_path << std::endl;
	std::ifstream html_file(url_path);
	std::ostringstream body_stream;
	if (!html_file.is_open()){
		std::cerr << RED << "Error: Could not open file: " << url_path << std::endl;
		std::string error_page = "error_pages/404.html";
		std::ifstream html_error_404(error_page);
		if (!html_error_404.is_open()){
			std::cerr << RED << "Error: Could not open error page: " << error_page << std::endl;
			return "<html><body><h1>404 Not Found</h1></body></html>";
		}
		body_stream << html_error_404.rdbuf();
		std::string body = body_stream.str();
		html_error_404.close();
		return body;
	}
	body_stream << html_file.rdbuf();
	std::string body = body_stream.str();
	html_file.close();
	return body;
}

std::string Webserver::findRoot(int client_fd, const std::string& url_path){
	std::string root;
	//1) Find server by client fd:
	Server* server = getServerBySocketFD(client_server_map.find(client_fd)->second);
	if (server == NULL){
		std::cerr << RED << "Error: Server not found for client fd: " << client_fd << std::endl;
		return "";
	}
	//2) Get Locations from server:
	std::unordered_map<std::string, Location> locations = server->getServerConfig().getLocations();
	if (locations.empty()){
		std::cerr << RED << "Error: No locations found for server fd: " << client_fd << std::endl;
		return "";
	}
	//3) Find location by url path (logenst prefix match):
	std::string matched_prefix;
	for (auto it = locations.begin(); it != locations.end(); ++it){
		if (url_path.find(it->first) == 0){
			if (it->first.size() > matched_prefix.size())
				matched_prefix = it->first;
		}
	}
	//4) Get root from location:
	if (!matched_prefix.empty()){
		root = locations[matched_prefix].getRoot();
		if (root.empty()){
			std::cerr << RED << "Error: No root found for url path: " << url_path << std::endl;
			return "";
		}
	}
	//5) If no location found, return default root:
	else{
		std::cerr << RED << "Error: No location found for url path: " << url_path << std::endl;
		root = "www/";
	
	}
	//6) Return root:
	return root;
}

int Webserver::handle_get_request(int client_fd, const std::string& url_path){
	std::cout << "GET request for: " << url_path << std::endl;
	std::string status_line = "HTTP/1.1 200 OK\r\n";
	std::string body;
	// std::string root = findRoot(client_fd, url_path);
	if (url_path == "/")
		body = build_response_body("www/html" + url_path + ".html");
	else
		body = build_response_body("www/html" + url_path);
	// std::cout << "Body: " << body << std::endl; //temporary, should be removed
	std::string response = status_line;
	response += "Content-Type: text/html\r\n";
	response += "Content-Length: " + std::to_string(body.size()) + "\r\n";
	response += "Connection: close\r\n";
	response += "\r\n";
	std::cout << "Response: " << response << std::endl; //temporary, should be removed
	response += body;
	//Send response to client:
	int bytes_sent = send(client_fd, response.c_str(), response.size(), 0);
	if (bytes_sent < 0){
		std::cerr << RED << "Error sending response to client: " << client_fd << std::endl;
		return 1;
	}
	std::cout << GREEN << "Response sent to client: " << client_fd << std::endl;
	//Close connection:
	if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_DEL, client_fd, NULL)< 0)
		std::cerr << RED << "Error removing client fd from epoll interest list: " << client_fd << " - " << strerror(errno) << std::endl;
	else
		std::cout << GREEN << "Successfully removed client fd from epoll interest list: " << client_fd << std::endl;
	this->getServerBySocketFD(this->client_server_map.find(client_fd)->second)->removeClient(client_fd);
	this->client_server_map.erase(client_fd);
	return 0;
}

//TODO: Store information somewhere.
int Webserver::handle_post_request(int client_fd, const std::string& url_path){
	(void)url_path;
	handle_success(client_fd);
	return 0;
}

//TODO: Use this to build the responses.
std::string Webserver::build_header(std::string body){
	std::string header = "Content-Type: text/html\r\n";
	header += "Content-Length: " + std::to_string(body.size()) + "\r\n";
	header += "Connection: close\r\n";
	header += "\r\n";
	return (header);
}

//TODO: Clean up send and building header (use previous method)
void Webserver::handle_success(int client_fd){
	Server* server = getServerBySocketFD(client_server_map[client_fd]);
	std::shared_ptr<Client> client = server->getclient(client_fd);
	std::unordered_map<std::string, std::string> clientRequest = client->get_RequestMap();
	std::string status_line = clientRequest["http_version"] + " 200 " + "OK" + "\r\n";
	std::string body = build_response_body("www/html/Success.html");
	std::string header = "Content-Type: text/html\r\n";
	header += "Content-Length: " + std::to_string(body.size()) + "\r\n";
	header += "Connection: close\r\n";
	header += "\r\n";
	std::string response = status_line;
	response += header;
	response += body;
	std::cout << "SUCCESS RESPONSE: " << response << std::endl;
	//Send response to client:
	int bytes_sent = send(client_fd, response.c_str(), response.size(), 0);
	if (bytes_sent < 0){
		std::cerr << RED << "Error sending response to client: " << client_fd << std::endl;
		return;
	}
	std::cout << GREEN << "Response sent to client: " << client_fd << std::endl;
	//Close connection:
	epoll_ctl(this->_epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
	this->client_server_map.erase(client_fd);
}

//TODO: Clean up send and building header (use previous method)
int Webserver::handle_error(int client_fd, int error_code, std::string error_message){
	std::cout << RED << "Error: " << error_code << std::endl;
	std::string status_line = "HTTP/1.1 " + std::to_string(error_code) + " " + error_message + "\r\n";
	std::string body = build_response_body("error_pages/" + std::to_string(error_code) + ".html");
	std::string header = "Content-Type: text/html\r\n";
	header += "Content-Length: " + std::to_string(body.size()) + "\r\n";
	header += "Connection: close\r\n";
	header += "\r\n";
	std::string response = status_line;
	response += header;
	response += body;
	std::cout << "Response: " << response << std::endl; //temporary, should be removed
	//Send response to client:
	int bytes_sent = send(client_fd, response.c_str(), response.size(), 0);
	if (bytes_sent < 0){
		std::cerr << RED << "Error sending response to client: " << client_fd << std::endl;
		return 1;
	}
	std::cout << GREEN << "Response sent to client: " << client_fd << std::endl;
	//Close connection:
	epoll_ctl(this->_epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
	this->client_server_map.erase(client_fd);
	return 0;
}

//Getters and Setters
const std::vector<Server>& Webserver::get_servers() const{
	return this->_servers;
}

Server* Webserver::getServerBySocketFD(int server_socket_fd){
	for (size_t i = 0; i < this->_servers.size(); ++i){
		if (this->_servers[i].getServerSocket() == server_socket_fd)
			return &this->_servers[i];
	}
	std::cout << RED << "Error: Server socket fd not found: " << server_socket_fd << std::endl;
	return NULL;
}

int Webserver::get_epoll_fd() const{
	return this->_epoll_fd;
}

//Utility methods
void Webserver::printServerFDs() const {
	std::cout << "Server FDs:" << std::endl;
	for (size_t i = 0; i < _servers.size(); ++i) {
		std::cout << "Server " << i << " FD: " << _servers[i].getServerSocket() << std::endl;
	}
}
 