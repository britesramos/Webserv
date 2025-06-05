#include "../include/Webserver.hpp"

//Constructor and Destructor
Webserver::Webserver(){
	this->_epoll_fd = 0;
}
Webserver::~Webserver(){
	close(_epoll_fd);
}

//Epoll methods:
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

int Webserver::addEpollFd(int new_connection_socket_fd, uint32_t events){
	if (new_connection_socket_fd <= 0){
		std::cout << RED << "Error: Invalid socket fd: " << new_connection_socket_fd << std::endl;
		return 1;
	}
	struct epoll_event event;
	event.data.fd = new_connection_socket_fd;
	event.events = events;
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
		for (int i = 0; i < epoll_num_ready_events; ++i){
			std::cout << "Event fd: " << events[i].data.fd << std::endl;
				//If the socket fd is the server socket fd there is a new connection:
			if (is_server_fd(events[i].data.fd) == true){
				if (accept_connection(this->_servers[i]) == 1)
					return 1;
			}
			//If the socket fd is a client socket fd, there is a request to read or a response to send:
			else {
					// CGI
					if (this->cgi_fd_to_client_map.count(events[i].data.fd)) {
					std::shared_ptr<Client>& client = this->cgi_fd_to_client_map[events[i].data.fd];

					// Read CGI response

						//EPOLLIN you read from CGI
						// if (events[i].events & EPOLLIN)
						// {
						// 	if (client->handle_cgi_response(*client->get_cgi()) == 0) {
						// 		// Remove CGI FD from epoll
						// 		epoll_ctl(this->_epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, nullptr);
						// 		close(events[i].data.fd); // Optional: close pipe if you're done with it
	
						// 		// Now modify the client FD to EPOLLOUT so response can be sent
						// 		struct epoll_event event;
						// 		event.events = EPOLLOUT;
						// 		if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_MOD, client->get_Client_socket(), &event) < 0) {
						// 			std::cerr << "Failed to switch Cgi Client to EPOLLOUT" << std::endl;
						// 			return 1;
						// 		}

						// } else if (events[i].events & EPOLLOUT)
						// {
						// 	//POST, you are going to send the body of your request to the CGI and then once you are done sending you change the status of EPOLL to EPOLLIN so you can read from the cgi and create a response for the client
						// }
						//EPOLLOUT you write the post body

						//if it is POST this will not work
						// }
						// if (EPOLLIN){
							int result = client->handle_cgi_response(*client->get_cgi());
							if (result == 1) {
							epoll_ctl(this->_epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, nullptr);
							close(events[i].data.fd);
							this->cgi_fd_to_client_map.erase(events[i].data.fd);

							struct epoll_event event;
							event.data.fd = client->get_Client_socket();
							event.events = EPOLLOUT;
							if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_MOD, client->get_Client_socket(), &event) < 0) {
								perror("epoll_ctl EPOLLOUT");
								return 1;
							}
							}
							else if (result == -1) {
								std::cerr << RED << "Error reading from CGI pipe" << std::endl;
								exit(1);  // test
								// handle error here 502, it will be handle in the end? or it will send the error here?
							}
						// }
						// else if (EPOLLOUT)
						// {
							
						// }
					}
				//client
				else if (events[i].events & EPOLLIN){
					std::cout << "EPOLLOUT event for client fd: " << events[i].data.fd << std::endl;
					if (process_request(events[i].data.fd) == -1)
						break ;
				}
				else if (events[i].events & EPOLLOUT){
					std::cout << "EPOLLOUT event for client fd: " << events[i].data.fd << std::endl;
					if (send_response(events[i].data.fd) == 1)
						return 1;
					Server* server = getServerBySocketFD(this->client_server_map.find(events[i].data.fd)->second);
					std::shared_ptr<Client>& client = server->getclient(events[i].data.fd);
					close_connection(client);
				}
			}
			//TODO: Find the client by fd and check if it has an error code:
			// if (client->get_error_code() != "200"){
			// 	handle_error(client);
			// }
		}
	}
		return 0;
}

//Send response to client:
int Webserver::send_response(int client_fd){
	Server* server = getServerBySocketFD(this->client_server_map.find(client_fd)->second);
	std::shared_ptr<Client>& client = server->getclient(client_fd);
	std::string response = client->get_Response();
	int bytes_sent = send(client->get_Client_socket(), response.c_str(), response.size(), 0);
	if (bytes_sent < 0){
		std::cerr << RED << "Error sending response to client: " << client->get_Client_socket() << std::endl;
		client->set_error_code("500");
		return 1;
	}
	std::cout << GREEN << "Response sent to client: " << client->get_Client_socket() << std::endl;
	return 0;
}

int Webserver::build_response(int client_fd){
	Server* server = getServerBySocketFD(this->client_server_map.find(client_fd)->second);
	std::shared_ptr<Client>& client = server->getclient(client_fd);
	if (client->get_Request("method") == "GET"){
		//if cgi request, handle it separately
		if (client->handle_get_request() == SUCCESS){
			struct epoll_event event;
			event.data.fd = client_fd;
			event.events = EPOLLOUT;
			if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_MOD, client_fd, &event) < 0){
				std::cout << RED << "Error changing EPOLLIN to EPOLLOUT for client: " << event.data.fd << std::endl;
				return 1;
			}
		}
	}
	// else if (client->get_Request("method") == "POST"){
	// 	//if cgi request, handle it separately
	// 	client->handle_post_request();
	// }
	// else if (client->get_Request("method") == "DELETE"){
	//if cgi request, handle it separately
	//else
	// 	client->handle_delete_request();
	// }
	return 0;
}

bool Webserver::is_server_fd(int fd){
	std::cout << "Checking if socket fd is a server socket fd: " << fd << std::endl;
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
	if (client_fd < 0) {
		std::cerr << "Invalid client_fd: " << client_fd << std::endl;
		return -1;
	}
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
	if (client->get_Request("url_path").find("/cgi-bin") != std::string::npos)
	{
		Cgi *cgi = new Cgi();
		client->set_cgi(cgi);
		std::cout << "CGI response" << std::endl;
		client->get_cgi()->start_cgi(getLocationByPath(client_fd, "/cgi-bin"));
		client->get_cgi()->run_cgi(*client);
		int cgi_out_fd = client->get_cgi()->get_cgi_out(READ);
		if (cgi_out_fd == -1)
		{
			std::cout << RED << "Error getting CGI out fd" << std::endl;
			return 1;
		}
		std::cout << "															CGI out fd: " << cgi_out_fd << std::endl;
		client->set_cgiOutputfd(cgi_out_fd); // TODO: Verificar necessity of this variable
		this->set_cgi_fd_to_client_map(cgi_out_fd, client);
		if (addEpollFd(cgi_out_fd, EPOLLIN) == -1)
		{
			std::cout << "Failed to add Cgi-out Fd to epoll" << std::endl;
			return 1;
		}
		client->set_isCgi(true); // TODO: Verify this as well
	}
	else
		build_response(client_fd);
	return 0;
}


int Webserver::accept_connection(Server& server){
	int new_connection_socket_fd;
	struct sockaddr_in new_connection_address;
	socklen_t new_connection_address_len = sizeof(new_connection_address);
	new_connection_socket_fd = accept(server.getServerSocket(), (struct sockaddr*)&new_connection_address, &new_connection_address_len);
	std::cout << GREEN << "New connection accepted: " << new_connection_socket_fd << std::endl;
	if (new_connection_socket_fd < 0)
	{
		std::cerr << RED << "Error accepting connection" << std::endl;
		return 1;
	}
	else
	{
		std::cout << "====== Processing Connection: " << new_connection_socket_fd << " ======" << std::endl;
		//ADD CLIENT TO EPOLL INTEREST LIST:
		if (this->addEpollFd(new_connection_socket_fd, EPOLLIN) == 1){
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


// //Handling Responses
// std::string Webserver::build_body(std::shared_ptr<Client>& client, const std::string& url_path, int flag){
// 	std::cout << "Building response body for: " << url_path << std::endl;
// 	//1)Find file to build body with:
// 	std::string root;
// 	if (flag == 1)
// 		root = url_path;
// 	else{
// 		if (url_path == "/")
// 			root = "www/html/.html";
// 		else{
// 			root = findRoot(client->get_Client_socket(), url_path);
// 			root += url_path;
// 		}
// 	}
// 	std::cout << "Root in build_body after findRoot : " << root << std::endl;
// 	//2)Opening file and converting into a string to be send back at the client:
// 	std::ifstream html_file(root);
// 	std::ostringstream body_stream;
// 	if (!html_file.is_open()){
// 		client->set_error_code("404");
// 		std::string body = body_stream.str();
// 		return body;
// 	}
// 	body_stream << html_file.rdbuf();
// 	std::string body = body_stream.str();
// 	html_file.close();
// 	return body;
// }

//TODO: Make sure that the headers look allways like this. If there are other things depending on the request????
// std::string Webserver::build_header(std::string body){
// 	std::string header = "Content-Type: text/html\r\n";
// 	header += "Content-Length: " + std::to_string(body.size()) + "\r\n";
// 	header += "Connection: close\r\n";
// 	header += "\r\n";
// 	return (header);
// }

// std::string Webserver::findRoot(int client_fd, const std::string& url_path){
// 	std::string root;
// 	Location locations;
// 	locations = getLocationByPath(client_fd, url_path);
// 	//TODO: Find a way to check if there is a location that matches that url_path.
// 	//1) If no location found, return default root:
// 	// else{
// 	// 	std::cerr << RED << "Error: No location found for url path: " << url_path << std::endl;
// 	// 	root = "www/";
// 	// }
// 	//2) Get root from location:
// 	root = locations.getRoot();
// 	if (root.empty()){
// 		std::cerr << RED << "Error: No root found for url path: " << url_path << std::endl;
// 		return "www/";
// 	}
// 	//3) Return root:
// 	return root;
// }

// std::string Webserver::build_status_line(std::shared_ptr<Client>& client, std::string status_code, std::string status_message){
// 	std::unordered_map<std::string, std::string> requestMap = client->get_RequestMap();
// 	std::string http_version = requestMap["http_version"];
// 	std::string status_line = http_version + " " + status_code + " " + status_message + "\r\n";
// 	return (status_line);
// }

// int Webserver::handle_get_request(std::shared_ptr<Client>& client, const std::string& url_path){
// 	if (is_method_allowed(client, url_path, "GET") == true){
// 		//1)Build response:
// 		std::cout << "GET request for: " << url_path << std::endl;
// 		std::string body = build_body(client, url_path, 0);
// 		if (body.empty())
// 			return 1;
// 		std::string header = build_header(body);
// 		std::string status_line = build_status_line(client , "200", "OK");
// 		std::string response = status_line + header;
// 		std::cout << "Response: " << response << std::endl;
// 		response += body;
		
// 		//2)Send response to client:
// 		int bytes_sent = send(client->get_Client_socket(), response.c_str(), response.size(), 0);
// 		if (bytes_sent < 0){
// 			std::cerr << RED << "Error sending response to client: " << client->get_Client_socket() << std::endl;
// 			client->set_error_code("500");
// 			return 1;
// 		}
// 		std::cout << GREEN << "Response sent to client: " << client->get_Client_socket() << std::endl;
// 	}
// 	return 0;
// }

void Webserver::close_connection(std::shared_ptr<Client>& client){
	//1)Remove from epoll interest list:
	if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_DEL, client->get_Client_socket(), NULL)< 0)
		std::cerr << RED << "Error removing client fd from epoll interest list: " << client->get_Client_socket() << " - " << strerror(errno) << std::endl;
	else
		std::cout << GREEN << "Successfully removed client fd from epoll interest list: " << client->get_Client_socket() << std::endl;
	//2)Remove from client_server_map:
	this->getServerBySocketFD(this->client_server_map.find(client->get_Client_socket())->second)->removeClient(client->get_Client_socket());
	this->client_server_map.erase(client->get_Client_socket());

}

// bool Webserver::is_method_allowed(std::shared_ptr<Client>& client, const std::string& url_path, std::string method){
// 	Location location = getLocationByPath(client->get_Client_socket(), url_path);
// 	std::vector<std::string> allowed_methods = location.get_methods();
// 	for (size_t i = 0; i < allowed_methods.size(); ++i){
// 		// std::cout << "method: " << allowed_methods[i] << std::endl; //TEMP
// 		if (allowed_methods[i] == method)
// 			return true;
// 	}
// 	std::cout << "METHOD NOT ALLOWED!" << std::endl; //What happens in this case? The website doesnt change?
// 	client->set_error_code("405");
// 	return false;
// }

//TODO: Store information somewhere. HOW TO HANDLE POST REQUESTS????
// int Webserver::handle_post_request(std::shared_ptr<Client>& client, const std::string& url_path){
// 	if (is_method_allowed(client, url_path, "POST") == true){
// 		std::unordered_map<std::string, std::string> Request_Map = client->get_RequestMap();
// 		if (Request_Map["Content-Type"] == "application/x-www-form-urlencoded")
// 			handle_post_form_request(client, url_path);
// 		(void)url_path;

// 		//1)Inspect content-type
// 		//2)Deal with request apropriatelly
// 		//3)Send response back
// 	}
// 	handle_success(client);
// 	return 0;
// }

// int Webserver::handle_post_form_request(std::shared_ptr<Client>& client, const std::string& url_path){
// 	std::string body = client->get_Request("body");
// 	//1)Decode URL and Parse form data into an unordered_map:
// 	std::unordered_map<std::string, std::string> decoded_url = decode_url(body); //TODO
// 	//2)Store data in the correct path:

// }


//TODO: Clean up send and building header (use previous method)
// void Webserver::handle_success(std::shared_ptr<Client>& client){
// 	//1)Build response:
// 	std::unordered_map<std::string, std::string> clientRequest = client->get_RequestMap();
// 	std::string status_line = clientRequest["http_version"] + " 200 " + "OK" + "\r\n";
// 	std::string body = build_body(client, "/Success.html", 0);
// 	std::string header = build_header(body);
// 	std::string response = status_line + header;
// 	std::cout << "SUCCESS RESPONSE: " << response << std::endl;
// 	response += body;
// 	//2)Send response to client:
// 	int bytes_sent = send(client->get_Client_socket(), response.c_str(), response.size(), 0);
// 	if (bytes_sent < 0)
// 		std::cerr << RED << "Error sending response to client: " << client->get_Client_socket() << std::endl;
// 	else
// 		std::cout << GREEN << "Response sent to client: " << client->get_Client_socket() << std::endl;
// }

// int Webserver::handle_error(std::shared_ptr<Client>& client){
// 	//1) Get error_code:
// 	std::string error_code = client->get_error_code();

// 	//2) Get error_message:
// 	int server_fd = this->client_server_map[client->get_Client_socket()];
// 	Server* server = getServerBySocketFD(server_fd);
// 	ServerConfig config = server->getServerConfig();
// 	std::map<std::string, std::string> error_pages = config.getErrorPages();
// 	std::string error_message = error_pages[error_code];

// 	//3) Build response to send to client:
// 	std::cout << RED << "Error: " << error_code << std::endl;
// 	std::string status_line = build_status_line(client, error_code, error_message);
// 	std::string body = build_body(client, "error_pages/" + error_code + ".html", 1);
// 	std::string header = build_header(body);
// 	std::string response = status_line + header;
// 	std::cout << "Response: " << response << std::endl; //temporary, should be removed
// 	response += body;
// 	//3) Send response to client:
// 	int bytes_sent = send(client->get_Client_socket(), response.c_str(), response.size(), 0);
// 	if (bytes_sent < 0){
// 		std::cerr << RED << "Error sending response to client: " << client->get_Client_socket() << std::endl;
// 		client->set_error_code("500");
// 		return 1;
// 	}
// 	std::cout << GREEN << "Response sent to client: " << client->get_Client_socket() << std::endl;
// 	return 0;
// }

//Getters and Setters
const std::vector<Server>& Webserver::get_servers() const{
	return this->_servers;
}

Server* Webserver::getServerByClientFD(int client_fd){
	int server_fd = this->client_server_map[client_fd];
	return (getServerBySocketFD(server_fd));
}

Server* Webserver::getServerBySocketFD(int server_socket_fd){
	for (size_t i = 0; i < this->_servers.size(); ++i){
		if (this->_servers[i].getServerSocket() == server_socket_fd)
			return &this->_servers[i];
	}
	std::cout << RED << "Error: Server socket fd not found: " << server_socket_fd << std::endl;
	return NULL;
}

std::shared_ptr<Client>& Webserver::getClientByClientFD(int client_fd){
	int server_fd = this->client_server_map[client_fd];
	Server* server = getServerBySocketFD(server_fd);
	std::shared_ptr<Client>& client = server->getclient(client_fd);
	return (client);
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

//TODO: To be deleted from here? Moved to ServerConfig.
Location Webserver::getLocationByPath(int client_fd, const std::string& url_path){
	//1) Find server by client fd:
	Server* server = getServerBySocketFD(client_server_map.find(client_fd)->second);
	if (server == NULL){
		std::cerr << RED << "Error: Server not found for client fd: " << client_fd << std::endl;
		return Location();
	}
	//2) Get Locations from server:
	std::unordered_map<std::string, Location> locations = server->getServerConfig().getLocations();
	if (locations.empty()){
		std::cerr << RED << "Error: No locations found for server fd: " << client_fd << std::endl;
		return Location();
	}
	//TODO: I think this is only matching the entire url. not also first section.
	//3) Find location by url path (logenst prefix match):
	std::string matched_prefix;
	for (auto it = locations.begin(); it != locations.end(); ++it){
		if (url_path.rfind(it->first, 0) == 0){
			if (it->first.size() > matched_prefix.size())
				matched_prefix = it->first;
		}
	}
	return (locations[matched_prefix]);
}

std::shared_ptr<Client> Webserver::get_client_by_cgi_fd(int cgi_fd)
{
	if (this->cgi_fd_to_client_map.find(cgi_fd) != this->cgi_fd_to_client_map.end())
		return this->cgi_fd_to_client_map[cgi_fd];
	else
		return nullptr;
}

void Webserver::set_cgi_fd_to_client_map(int cgi_fd, std::shared_ptr<Client> client)
{
	this->cgi_fd_to_client_map[cgi_fd] = client;
}

void Webserver::clean_up(){
	for (size_t i = 0; i < _servers.size(); ++i){
		const std::unordered_map<int, std::shared_ptr<Client> >& clients = _servers[i].getClients();
		for (std::unordered_map<int, std::shared_ptr<Client> >::const_iterator it = clients.begin(); it != clients.end(); ++it){
			close (it->first);
		}
	}
	for (size_t i = 0; i < this->_servers.size(); ++i){
		close(this->_servers[i].getServerSocket());
	}
	if (this->_epoll_fd > 0)
		close(this->_epoll_fd);
}