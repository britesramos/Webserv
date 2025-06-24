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
		std::cout << RED << "Error(addEpollFd): Invalid socket fd: " << new_connection_socket_fd << std::endl;
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

int Webserver::removeEpollFd(int socket_fd, uint32_t events){
	if (socket_fd <= 0){
		std::cout << RED << "Error(removeEpollFd): Invalid socket fd: " << socket_fd << std::endl;
		return 1;
	}
	struct epoll_event event;
	event.data.fd = socket_fd;
	event.events = events;
	if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_DEL, socket_fd, &event) < 0){
		std::cout << RED << "Error removing socket fd from epoll interest list: " << event.data.fd << std::endl;
		return 1;
	}
	this->_epoll_event_count--;
	std::cout << GREEN << "Removed socket fd from epoll interest list: " << event.data.fd << std::endl;
	return 0;
}

int Webserver::modifyEpollEvent(int socket_fd, uint32_t events){
	struct epoll_event event;
	event.data.fd = socket_fd;
	event.events = events;
	if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_MOD, socket_fd, &event) < 0){
		std::cout << RED << "Error changing EPOLLIN to EPOLLOUT for client: " << event.data.fd << std::endl;
		return 1;
	}
	return 0;
}


int Webserver::epoll_wait_util(struct epoll_event* events){
	//EPOLL_WAIT:
	int epoll_num_ready_events = epoll_wait(this->_epoll_fd, events, this->_epoll_event_count, TIMEOUT);
	if (epoll_num_ready_events == -1)
	{
		std::cerr << RED << "Error waiting for epoll events" << std::endl;
		return -1;
	}
	if (epoll_num_ready_events > 0) {
	std::cout << GREEN << "			Number of ready events: " << epoll_num_ready_events << std::endl;
	}
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

int Webserver::main_loop()
{
	std::cout << "\n====== !!! WEBSERV PARTY TIME !!! ======\n" << std::endl;
	while(true)
	{
		//Check if epoll interest list is empty:
		if (this->_epoll_event_count == 0){
			std::cout << RED << "No epoll fds on interest list.\n\n*****Server closing*****" << std::endl;
			break ;
		}
		//EPOLL_WAIT:
		// std::cout << GREEN << "\n\n=======================================================\n=============== !!! EPOLL_WAIT TIME !!! ===============\n=======================================================" << std::endl;
		struct epoll_event events[this->_epoll_event_count];
		int epoll_num_ready_events = epoll_wait_util(events);
		if (epoll_num_ready_events == -1)
			return 1;

		// Timeout checks:
		timeout_checks();

		//PROCESSING EVENTS:
		std::cout << GREEN << "\n\n=============== !!! PROCESSING EVENTS !!! ===============" << std::endl;

		for (int i = 0; i < epoll_num_ready_events; ++i)
		{
			std::cout << "Event fd: " << events[i].data.fd << ", Events: " << events[i].events << std::endl;
			//If the socket fd is the server socket fd there is a new connection:
			if (is_server_fd(events[i].data.fd) == true){
				if (accept_connection(this->_servers[i]) == 1)
					return 1;
			}
			//If the socket fd is a client socket fd, there is a request to read or a response to send:
			else
			{
				// 1. Handle CGI input (write) FDs for POST body
				if (this->cgi_input_fd_to_client_map.count(events[i].data.fd)) {
					std::shared_ptr<Client> client = get_client_by_cgi_input_fd(events[i].data.fd);
					std::string& buffer = client->get_cgiInputBuffer();
					size_t written = client->get_cgiInputWritten();
					int fd = client->get_cgiInputfd();

					// std::cout << YELLOW << "Writing to CGI: " << buffer.size() << " bytes, written so far: " << written << std::endl;
					ssize_t n = write(fd, buffer.data() + written, buffer.size() - written);
					if (n > 0) {
						written += n;
						// std::cout << YELLOW << "write() returned: " << n << ", written now: " << written << ", buffer.size(): " << buffer.size() << std::endl;
						client->set_cgiInputWritten(written);
						if (written == buffer.size()) {
							removeEpollFd(fd, EPOLLOUT);
							close(fd);
							client->set_cgiInputfd(-1);
							this->cgi_input_fd_to_client_map.erase(fd);
							std::cout << YELLOW << "Finished writing POST body to CGI for client fd: " << client->get_Client_socket() << std::endl;
						}
					} else if (n < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
						// std::cerr << "write() failed, errno: " << errno << " (" << strerror(errno) << ")" << std::endl;
						removeEpollFd(fd, EPOLLOUT);
						close(fd);
						client->set_cgiInputfd(-1);
						this->cgi_input_fd_to_client_map.erase(fd);
						client->set_error_code("502");
						modifyEpollEvent(client->get_Client_socket(), EPOLLOUT);
						std::cerr << RED << "Error writing POST body to CGI for client fd: " << client->get_Client_socket() << std::endl;
					}
				}
				// 2. Handle CGI output (read)
				else if (this->cgi_fd_to_client_map.count(events[i].data.fd)) {
					std::shared_ptr<Client>& client = this->cgi_fd_to_client_map[events[i].data.fd];
					int result = client->handle_cgi_response(*client->get_cgi());
					if (result == 1) {
						close(events[i].data.fd);
						this->cgi_fd_to_client_map.erase(events[i].data.fd);
						modifyEpollEvent(client->get_Client_socket(), EPOLLOUT);
						delete client->get_cgi();
					}
					else if (result == -1) {
						std::cerr << RED << "Error reading from CGI pipe" << std::endl;
						client->set_error_code("502");
						close(events[i].data.fd);
						this->cgi_fd_to_client_map.erase(events[i].data.fd);
						modifyEpollEvent(client->get_Client_socket(), EPOLLOUT);
						delete client->get_cgi();
					}
				}
				// 3. Handle client socket (EPOLLIN/EPOLLOUT)
				else if (events[i].events & EPOLLIN){
					std::cout << GREEN << "EPOLLIN event for client fd: " << events[i].data.fd << std::endl;
					if (process_request(events[i].data.fd) == -1)
						break ;
				}
				else if (events[i].events & EPOLLOUT){
					std::cout << GREEN << "EPOLLOUT event for client fd: " << events[i].data.fd << std::endl;
					send_response(events[i].data.fd);
					std::cout << GREEN << "Closing connection for client fd: " << events[i].data.fd << std::endl;
					close_connection(events[i].data.fd);
				}
			}
		}
	}
	return 0;
}

void Webserver::timeout_checks() {
	auto now = std::chrono::steady_clock::now();

	// 1. Client timeout
	for (size_t s = 0; s < _servers.size(); ++s) {
		auto& clients = _servers[s].getClients();
			for (auto it = clients.begin(); it != clients.end(); ) {
			std::shared_ptr<Client> client = it->second;
			auto current = it++; // increment iterator before erase
			auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - client->get_activity()).count();
			if (elapsed > CLIENT_TIMEOUT) {
				send_response(client->get_Client_socket());
				close_connection(client->get_Client_socket());
				clients.erase(current);
			}
		}
	}

	// 2. CGI timeout (activity and hard timeout)
	for (auto it = cgi_fd_to_client_map.begin(); it != cgi_fd_to_client_map.end(); ) {
		std::shared_ptr<Client> client = it->second;
		Cgi* cgi = client->get_cgi();
		if (cgi) {
			auto elapsed_activity = std::chrono::duration_cast<std::chrono::seconds>(now - cgi->get_activity()).count();
			auto elapsed_start = std::chrono::duration_cast<std::chrono::seconds>(now - cgi->get_start_time()).count();
			std::cout << YELLOW << "CGI for client " << client->get_Client_socket() << " elapsed_activity: " << elapsed_activity << "s, elapsed_start: " << elapsed_start << "s." << std::endl;

			if (elapsed_activity > CGI_TIMEOUT || elapsed_start > CGI_TIMEOUT * 6) {
				std::cout << RED << "CGI for client " << client->get_Client_socket() << " timed out." << std::endl;
					if (cgi->get_pid() > 0) {
					kill(cgi->get_pid(), SIGKILL);
					waitpid(cgi->get_pid(), NULL, 0); // Clean up zombie process
					std::cout << RED << "Killed CGI process with PID: " << cgi->get_pid() << std::endl;
				}
				client->set_error_code("504");
				close(client->get_cgiOutputfd());
				it = cgi_fd_to_client_map.erase(it);
				send_response(client->get_Client_socket());
				close_connection(client->get_Client_socket());
				continue;
			}
		}
		++it;
	}
	}

//Send response to client:
int Webserver::send_response(int client_fd){
	Server* server = getServerBySocketFD(this->client_server_map.find(client_fd)->second);
	std::shared_ptr<Client>& client = server->getclient(client_fd);
	if (client->get_error_code() != "200")
	{
		client->handle_error();
	}
	std::string response = client->get_Response();
	int bytes_sent = send(client->get_Client_socket(), response.c_str(), response.size(), 0);
	if (bytes_sent < 0){
		std::cerr << RED << "Error sending response to client: " << client->get_Client_socket() << std::endl;
		client->set_error_code("500"); //I am not sure if this can be checked. The way to send the response is trough send() if it doenst work for the correct response it will not work for the 500 html file either.
		return 1; //Exit???
	}
	client->update_activity(); // for timeout checks
	std::cout << GREEN << "Response sent to client: " << client->get_Client_socket() << std::endl;
	return 0;
}

//Refactor this method. Not needing SUCCESS return anymore. I think.
int Webserver::build_response(int client_fd){
	Server* server = getServerBySocketFD(this->client_server_map.find(client_fd)->second);
	std::shared_ptr<Client>& client = server->getclient(client_fd);

    std::string path = client->findRoot(client->get_Request("url_path")) + client->get_Request("url_path");
    // std::cout << "      PATH: " << path << std::endl;
    if (access(path.c_str(), F_OK) != 0) {
        std::cerr << RED << "Path not found: " << path << RESET << std::endl;
        client->set_error_code("404");
        modifyEpollEvent(client_fd, EPOLLOUT); // should remove and leave it for the second if to take care?
        return 1;
    }
	if (client->get_error_code() != "200"){
		modifyEpollEvent(client_fd, EPOLLOUT);
		return 1;
	}
	if (client->get_Request("method") == "GET"){
		if (client->handle_get_request() == SUCCESS)
			modifyEpollEvent(client_fd, EPOLLOUT);
		// else
		// 	close_connection(client_fd);
	}
	else if (client->get_Request("method") == "POST"){
		if (client->handle_post_request() == SUCCESS)
			modifyEpollEvent(client_fd, EPOLLOUT);
		// else
		// 	close_connection(client_fd);
	}
	else if (client->get_Request("method") == "DELETE"){
		if (client->handle_delete_request() == SUCCESS)
			modifyEpollEvent(client_fd, EPOLLOUT);
		// else
		// 	close_connection(client_fd);
	}
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

static bool is_method_correct(std::shared_ptr<Client>& client)
{
	std::string method = client->get_Request("method");
	if (method.empty()) {
		std::cerr << RED << "Method not found in request" << RESET << std::endl;
		client->set_error_code("400");
		// modifyEpollEvent(client_fd, EPOLLOUT);
		// delete cgi;
		// client->set_cgi(nullptr);
		return false;
	}
	if ((method == "POST" && !client->get_cgi()->get_method_post()) || (method == "GET" && !client->get_cgi()->get_method_get()) || (method == "DELETE" && !client->get_cgi()->get_method_del()))
	{
		client->set_error_code("405");
		// modifyEpollEvent(client_fd, EPOLLOUT);
		// delete cgi;
		// client->set_cgi(nullptr);
		return false;
	}
	return true;
}

static bool is_cgi_path_valid(std::shared_ptr<Client>& client)
{
	std::string url_path = client->get_Request("url_path");
	std::string cgi_script_path = client->get_cgi()->get_config_root() + url_path;
	if (access(cgi_script_path.c_str(), F_OK) != 0) {
		std::cerr << RED << "CGI script not found: " << cgi_script_path << RESET << std::endl;
		client->set_error_code("404");
		// delete cgi;
		// client->set_cgi(nullptr);
		return false;
	}
	if (url_path.size() >= 3 && url_path.compare(url_path.size() - 3, 3, ".py") != 0){
		std::cerr << RED << "Access FORBIDDEN to CGI Folder" << RESET << std::endl;
		client->set_error_code("403");
		return false;
	}
	return true;
}

bool Webserver::handle_cgi_post_and_delete(std::shared_ptr<Client>& client)
{
	std::string method = client->get_Request("method");
	if ((method == "POST" && client->get_cgi()->get_method_post()) || (method == "DELETE" && client->get_cgi()->get_method_del())) {
		int cgi_in_fd = client->get_cgi()->get_cgi_in(WRITE);
		client->set_cgiInputfd(cgi_in_fd);
		client->set_cgiInputBuffer(client->get_Request("body"));
		client->set_cgiInputWritten(0);

		// 1. Execute CGI script
		client->get_cgi()->run_cgi(*client);

		// 2. Add write-end to epoll for writing POST body
		set_cgi_input_fd_to_client_map(cgi_in_fd, client);
		addEpollFd(cgi_in_fd, EPOLLOUT);

		// 3. Set up output fd for reading CGI response
		int cgi_out_fd = client->get_cgi()->get_cgi_out(READ);
		if (cgi_out_fd == -1) {
			std::cout << RED << "Error getting CGI out fd" << std::endl;
			return false;
		}
		client->set_cgiOutputfd(cgi_out_fd);
		this->set_cgi_fd_to_client_map(cgi_out_fd, client);
		if (addEpollFd(cgi_out_fd, EPOLLIN) == -1) {
			std::cout << "Failed to add Cgi-out Fd to epoll" << std::endl;
			return false;
		}
		return true;
	}
	return false;
}

bool Webserver::processing_cgi(std::shared_ptr<Client>& client, int client_fd)
{
	std::string url_path = client->get_Request("url_path");
	if (url_path.compare("/cgi-bin") == 0 || url_path.compare("/cgi-bin/") == 0)
	{
		std::cerr << RED << "Access FORBIDDEN to CGI Folder" << RESET << std::endl;
		client->set_error_code("403");
		modifyEpollEvent(client_fd, EPOLLOUT);
		return true;
	}
	if (url_path.find("/cgi-bin") != std::string::npos)
	{
		Cgi *cgi = new Cgi();
		client->set_cgi(cgi);
		std::cout << "CGI response" << std::endl;
		client->get_cgi()->start_cgi(getLocationByPath(client_fd, "/cgi-bin"));
		if (is_cgi_path_valid(client) == false || is_method_correct(client) == false)
		{
			modifyEpollEvent(client_fd, EPOLLOUT);
			return true;
		}

		if (handle_cgi_post_and_delete(client))
			return true;
		
		client->get_cgi()->run_cgi(*client);
		int cgi_out_fd = client->get_cgi()->get_cgi_out(READ);
		if (cgi_out_fd == -1)
		{
			std::cout << RED << "Error getting CGI out fd" << std::endl;
			return false;
		}
		client->set_cgiOutputfd(cgi_out_fd);
		this->set_cgi_fd_to_client_map(cgi_out_fd, client);
		if (addEpollFd(cgi_out_fd, EPOLLIN) == -1)
		{
			std::cout << "Failed to add Cgi-out Fd to epoll" << std::endl;
			return false;
		}
		return true;
	}
	return false;
}

int Webserver::process_request(int client_fd){
	if (client_fd < 0) {
		std::cerr << "Invalid client_fd: " << client_fd << std::endl;
		return -1;
	}
	int this_client_server = this->client_server_map.find(client_fd)->second;
	std::cout << "this_client_server: " << this_client_server << std::endl;
	std::shared_ptr<Client>& client = getServerBySocketFD(this_client_server)->getclient(client_fd);
	
	std::cout << GREEN << "\n====== Processing Client: " << client_fd << " ======" << std::endl;
	int bytes_received = 0;
	char buffer[BUFFER_SIZE] = {0}; //TODO: Fix this to parse the entire request, we are currently only reading a fixed BUFFER_SIZE
	bytes_received = recv(client_fd, buffer, BUFFER_SIZE, 0); //TODO: check Flags
	client->update_activity(); // for timeout checks
	std::cout << "Bytes received: " << bytes_received << std::endl;

	if (bytes_received == 0) // Connection closed by client
	{
		std::cout << "Client closed connection: " << client_fd << std::endl;
		close_connection(client_fd);
		return 0;
	}

	if (bytes_received < 0) //An error ocurred while recv().
	{
		std::cout << RED << "Error receiving data from client: " << client_fd << " - " << strerror(errno) << std::endl;
		client->set_error_code("400");
		return 1;
	}
	client->appendToBufferRequest(std::string(buffer, bytes_received));
	if (client->get_requestBuffer().find("Content-Length: ") != std::string::npos && bytes_received != 0) {
        // Check if we've received the complete POST data
        size_t content_length_pos = client->get_requestBuffer().find("Content-Length: ");
        if (content_length_pos != std::string::npos) {
            size_t content_length = std::stoi(client->get_requestBuffer().substr(content_length_pos + 16));
            size_t header_end = client->get_requestBuffer().find("\r\n\r\n");
            
            if (header_end != std::string::npos) {
                size_t body_length = client->get_requestBuffer().length() - (header_end + 4);
                if (body_length >= content_length) {
					std::cout << "REQUEST: " << client->get_requestBuffer() << std::endl;
                    // We have received all the POST data, proceed to build response
                    client->parseClientRequest();
					if (processing_cgi(client, client_fd))
					{
						std::cout << "Processing CGI request for client fd: " << client_fd << std::endl;
						return 0;
					}
					else
						build_response(client_fd);
                }
                return 0; // Still receiving data
            }
        }
        return 0; // Still receiving headers
    }
	else {//Ready to process request. Received the entire request.
		std::cout << "REQUEST: " << client->get_requestBuffer() << std::endl;
		//Parse client request into correct client object:
		client->parseClientRequest();
		// if (client->get_Request("Content-Leght") == client->get_requestBuffer().size())
		if (processing_cgi(client, client_fd))
		{
			std::cout << "Processing CGI request for client fd: " << client_fd << std::endl;
			return 0;
		}
		else
			build_response(client_fd);
	} 
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
		fcntl(new_connection_socket_fd, F_SETFL, O_NONBLOCK);
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

void Webserver::close_connection(int connection_fd){
	Server* server = getServerBySocketFD(this->client_server_map.find(connection_fd)->second);
	//1)Remove from epoll interest list:
	removeEpollFd(connection_fd, 0);
	//2)Remove client from server unordered_map:
	server->removeClient(connection_fd);
	//3)Remove from client_server_map:
	this->client_server_map.erase(connection_fd);
	//4)Close fd:
	close(connection_fd);
}

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

void Webserver::set_cgi_input_fd_to_client_map(int cgi_in_fd, std::shared_ptr<Client> client) {
	this->cgi_input_fd_to_client_map[cgi_in_fd] = client;
}

std::shared_ptr<Client> Webserver::get_client_by_cgi_input_fd(int cgi_in_fd) {
	if (cgi_input_fd_to_client_map.count(cgi_in_fd))
		return cgi_input_fd_to_client_map[cgi_in_fd];
	return nullptr;
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

//Utility methods
void Webserver::printServerFDs() const {
	std::cout << "Server FDs:" << std::endl;
	for (size_t i = 0; i < _servers.size(); ++i) {
		std::cout << "Server " << i << " FD: " << _servers[i].getServerSocket() << std::endl;
	}
}