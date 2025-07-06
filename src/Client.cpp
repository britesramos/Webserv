#include "../include/Client.hpp"
#include "../include/Cgi.hpp"

Client::Client(int socket_fd, ServerConfig& server_config):_Client_socket(socket_fd), _server_config(server_config), cgi(nullptr){
	this->_error_code = "200";
	this->last_activity = std::chrono::steady_clock::now();
	std::cout << GREEN << "Client Request received -- " << this->_Client_socket << std::endl;
}

Client::~Client(){
	std::cout << "Client Request ---" << this->_Client_socket << "--- closed" << std::endl;
	delete this->cgi;
}

Client& Client::operator=(const Client& other){
	if (this != &other)
	{
		this->_Client_socket = other._Client_socket;
		this->_error_code = other._error_code;
		this->_request_buffer = other._request_buffer;
		this->_Client_RequestMap = other._Client_RequestMap;
		this->_response = other._response;
		this->_server_config = other._server_config;
		this->last_activity = other.last_activity;
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
	if (this->_request_buffer.find("Content-Length:") != std::string::npos)
	{
		size_t content_lenght = std::stoul(this->_Client_RequestMap["Content-Length:"]);
		if (content_lenght > static_cast<size_t>(_server_config.getMaxClientSize()))
		{
			std::cout << RED << "Content lenght exceeds maximum client size." << std::endl;
			this->_error_code = "413";
			return -1;
		}
		else if (parse_body() < 0)
		{
			std::cerr << RED << "Error parsing body of request" << std::endl;
			return -1;
		}
	}
	return 0;
}

//Building response methods:
int Client::handle_get_request(){
	std::string url_path = this->get_Request("url_path");
	if (is_method_allowed(url_path, "GET") == true){
		std::string response;
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
		this->_response = response;
	}
	return 0;
}

int Client::handle_delete_request(){
	std::string url_path = this->get_Request("url_path");
	std::cout << YELLOW << "URL_PATH:::: " << url_path << std::endl;
	std::string body = this->get_Request("body");
	std::cout << YELLOW << "BODY:::: " << body << std::endl;
	if (is_method_allowed(url_path, "DELETE") == true){
		std::string root = findRoot(url_path);
		std::string directory_path = root + url_path;
		std::cout << YELLOW << directory_path << std::endl;		

		//1)If directory_path doesn't exist return with error:
		if (!std::filesystem::exists(directory_path)){
			std::cout << RED << "404 | File/Directory path not found: " << directory_path << std::endl;
			this->_error_code = "404";
			return 0;
		}
		//2)If delete request doesn't have body, delete file/directory path:
		if (this->_Client_RequestMap["body"].empty())
		{
			std::error_code error;
			if(std::filesystem::is_directory(directory_path))
				std::filesystem::remove_all(directory_path, error);
			else
				std::filesystem::remove(directory_path, error);
			if (error){
				std::cout << RED << "Failed to delete: " << directory_path << std::endl;
				this->_error_code = "500";
				return 0;
			}
		}
		//3)Look for email in database/delete if found:
		if (this->_Client_RequestMap["Content-Type:"] == "application/json" && !body.empty()){
			std::string email;
			size_t end_pos = body.find_last_of("\"");
			size_t newstr_len = body.length() - (body.length() - end_pos) - 10;
			if (end_pos != std::string::npos)
				email = body.substr(10, newstr_len);
			else
				return 1;
			std::cout << YELLOW << "EMAIL: " << email << std::endl;
			
			std::ifstream read_file(directory_path.c_str());
			if (!read_file.is_open()){
				std::cout << RED << "Failed to open file to reading: " << directory_path << std::endl;
				this->_error_code = "500";
				return 0;
			}
			std::stringstream buffer;
			buffer << read_file.rdbuf();
			std::string content = buffer.str();
			read_file.close();
			if(content.find(email) != std::string::npos){
				std::stringstream new_content;
				std::string line;
				std::istringstream content_stream(content);

				while (std::getline(content_stream, line)) {
					if (line.find(email) == std::string::npos) {
						new_content << line << "\n";
					}
				}
				std::ofstream write_file(directory_path.c_str(), std::ios::trunc);
				if (!write_file.is_open()){
					std::cout << RED << "Failed to open file for writing: " << directory_path << std::endl;
					this->_error_code = "500";
					return 0; 
				}
				write_file << new_content.str();
				write_file.close();
				std::cout << GREEN << "Successfully delted entry with email: " << email << std::endl;
			}
			else{
				std::cout << RED << "This email does not exist in our database. It can't be deleted." << RESET << std::endl;
                return 0;
            }

		}
		handle_success();
	}
	return 0;
}

int Client::handle_post_request(){
	std::string url_path = this->get_Request("url_path");
	std::string root = findRoot(url_path);
	if (is_method_allowed(url_path, "POST") == true){
		//Check content type webserver supported:
		if (this->get_Request("Content-Type:") != "application/x-www-form-urlencoded"){
			std::cout << RED << "415 | Unsupported Media Type" << std::endl;
			this->_error_code = "415";
			return 0;
		}
		//1)If folder doesnt exist create:
		//1.1) Extract folder path:
		size_t last_slash = url_path.find_last_of('/');
		std::string directory_path;
		if (last_slash != std::string::npos)
			directory_path = root + url_path.substr(0, last_slash);
		else
			directory_path = root;
		//1.2) If it doesnt exist create folder:
		struct stat stat_buf;
		if (stat(directory_path.c_str(), &stat_buf) == -1){
			if(mkdir(directory_path.c_str(), 0777) == -1){
				std::cout << RED << "Failed to create directory: " << directory_path << std::endl;
				this->_error_code = "500";
				return 0;
			}
		}
		//1.4) Create file if it doesnt exist:
		std::string file_path = root + url_path;
		std::ofstream file(file_path.c_str(), std::ios::app);
		if (!file.is_open()){
			std::cout << RED << "Failed to open/create file: " << file_path << std::endl;
			this->_error_code = "500";
			return 0;
		}
		file.close();
		//2)If entry doenst exist already include it:
		//2.1)Parse form data:
		std::string body = this->_Client_RequestMap["body"];
		std::vector<std::string> temp;
		std::vector<std::string> entries;
		size_t pos1 = 0;
		while ((pos1 = body.find('&')) != std::string::npos){
			temp.push_back(body.substr(0, pos1));
			body.erase(0, pos1 + 1);
		}
		if (!body.empty()){
			temp.push_back(body);
		}
		for (size_t i = 0; i < temp.size(); ++i){
			size_t pos2 = temp[i].find('=');
			if(pos2 != std::string::npos){
				std::string tmp = temp[i].substr(pos2 + 1);
				entries.push_back(tmp);
			}
		}
		for (const auto& entry : entries) {
			std::cout << "Entry: " << entry << std::endl;
		}

		//2.2)Check if entry already exists if not add it to the file:
		std::string to_look_for;
		size_t i = 0;
		while(i < entries.size() - 1){
			to_look_for += url_decode(entries[i]);
			to_look_for += ",";
			++i;
		}
		to_look_for += url_decode(entries[i]);
		std::ifstream read_file(file_path.c_str());
		if (!read_file.is_open()) {
			std::cout << RED << "Failed to open file for reading: " << file_path << std::endl;
			this->_error_code = "500";
			return 0;
		}
		std::stringstream buffer;
		buffer << read_file.rdbuf();
		std::string content = buffer.str();
		read_file.close();
		std::cout << YELLOW << "CONTENT: " << content << std::endl;
		if (content.find(to_look_for) != std::string::npos) {
			std::cout << YELLOW << "This entry already exists: " << to_look_for << std::endl;
			this->_error_code = "409"; //I dont think this is the right way to deal with this.
			return 0;
		}
		else{
			std::ofstream write_file(file_path.c_str(), std::ios::app);
			if (!write_file.is_open()) {
				std::cout << RED << "Failed to open file for writing: " << file_path << std::endl;
				this->_error_code = "500";
				return 0;
			}
			write_file << to_look_for << "\n";
			write_file.close();
			std::cout << GREEN << "Successfully added new entry: " << to_look_for << std::endl;
		}
		handle_success();
	}
	return 0;
}

std::string Client::url_decode(const std::string& encoded) {
    std::string decoded = encoded;
    std::string::size_type pos = 0;

    // Replace %40 with @
    while ((pos = decoded.find("%40", pos)) != std::string::npos) {
        decoded.replace(pos, 3, "@");
        pos += 1;
    }

    // Reset position for next replacement if needed
    pos = 0;
    
    // Replace %20 with space
    while ((pos = decoded.find("%20", pos)) != std::string::npos) {
        decoded.replace(pos, 3, " ");
        pos += 1;
    }

    return decoded;
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
		std::string full_response = status_line + cgi_output_buffer;

		std::cout << "Final CGI Response:\n" << full_response << std::endl;
		this->_response = full_response;

		// check for error in python! trash trash trash
		if (cgi_output_buffer.find("Traceback") != std::string::npos) {
			size_t trace_pos = cgi_output_buffer.find("Traceback");
			std::string traceback = cgi_output_buffer.substr(trace_pos);
			std::cerr << "CGI Error:\n" << traceback << std::endl;
			this->set_error_code("500");
			return -1;
		}
		this->cgi_output_buffer.clear();
		return 1;
	}
	else {
		buffer[bytes_read] = '\0';
		cgi_output_buffer += buffer;
		cgi.update_activity();
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

std::string Client::findRoot(const std::string& url_path){
	//1)Find longest prefix match:
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

bool Client::autoindex_return(const std::string& url_path){

	bool autoindex;
	std::unordered_map<std::string, Location> locations; // TODO: create a function for this locations block best match
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

	autoindex = location.getAutoindex();

	return autoindex;
}

std::string Client::find_Index(const std::string& url_path){

	std::string index;
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

	index = location.getIndex();
	if (index.empty()){
		std::cerr << YELLOW << "No Page Index Set for Directory " << url_path << std::endl;
	}

	return index;
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
	const std::vector<std::string> allowed_methods = location.get_methods();
	for (size_t i = 0; i < allowed_methods.size(); ++i){
		if (allowed_methods[i] == method)
			return true;
	}
	std::cout << RED << "METHOD NOT ALLOWED!" << std::endl;
	this->_error_code = "405";
	return false;
}

void Client::handle_return_page(std::string code, std::string url)
{
    std::stringstream body;
    body << "<html><head><title>" << code << " Moved Permanently</title></head>"
        << "<body><p>Redirecting to <a href=\"" << url << "\">" << url << "</a>.</p></body></html>";
    std::string status_line = build_status_line(code, "Moved Permanently");
    std::string header = "Location: " + url + "\r\n" + build_header(body.str());
    std::string response = status_line + header + body.str();
    this->_response = response;
}

static std::string generate_autoindex_html(const std::string& dir_path, const std::string& url_path) {
    DIR* dir = opendir(dir_path.c_str());
    if (!dir)
		return "<html><body>Unable to open directory</body></html>";

    std::stringstream html;
    html << "<html><head><title>Index of " << url_path << "</title></head><body>";
    html << "<h1>Index of " << url_path << "</h1><ul>";

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        std::string name = entry->d_name;
        if (name == ".") continue;
        std::string link = url_path + (url_path.back() == '/' ? "" : "/") + name;
        html << "<li><a href=\"" << link << "\">" << name << "</a></li>";
    }
    closedir(dir);
    html << "</ul></body></html>";
    return html.str();
}

void Client::handle_autoindex_page(const std::string& dir_path, const std::string& url_path) {
    std::string body = generate_autoindex_html(dir_path, url_path);
    std::string header = build_header(body);
    std::string status_line = build_status_line("200", "OK");
    std::string response = status_line + header + body;
    this->_response = response;
}

void Client::handle_index_page(std::string full_path){
	std::string body = build_body(full_path, 1);
	std::string header = build_header(body);
	std::string status_line = build_status_line("200", "OK");
	std::string response = status_line + header;
	response += body;
	// std::cout << "SUCCESS RESPONSE: " << response << std::endl;
	this->_response = response;
}

void Client::handle_success(){
	std::string body = build_body("www/html/Success.html", 1);
	std::string header = build_header(body);
	std::string status_line = build_status_line("200", "OK");
	std::string response = status_line + header;
	response += body;
	// std::cout << "SUCCESS RESPONSE: " << response << std::endl;
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

void Client::handle_autoindex(){
	std::string body = build_body(this->findRoot(this->get_Request("url_path")) + this->get_Request("url_path"), 1);
	std::string header = build_header(body);
	std::string status_line = build_status_line("200", "OK");
	std::string response = status_line + header;
	response += body;
	// std::cout << "SUCCESS RESPONSE: " << response << std::endl;
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

int Client::get_cgiOutputfd()
{
	return this->CgiOutputfd;
}

int Client::get_cgiInputfd()
{
	return this->CgiInputfd;
}

Cgi* Client::get_cgi()
{
	return this->cgi;
}

std::string Client::get_requestBuffer(){
	return this->_request_buffer;
}

std::string& Client::get_cgiInputBuffer()
{
	return this->cgi_input_buffer;
}

size_t Client::get_cgiInputWritten() const
{
	return this->cgi_input_written;
}

std::chrono::steady_clock::time_point Client::get_activity() const {
	return this->last_activity;
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

void Client::set_cgiOutputfd(int fd)
{
	this->CgiOutputfd = fd;
}

void Client::set_cgiInputfd(int fd)
{
	this->CgiInputfd = fd;
}

void Client::set_cgi(Cgi* cgi)
{
	this->cgi = cgi;
}

void Client::set_cgiInputBuffer(const std::string& buffer)
{
	this->cgi_input_buffer = buffer;
}

void Client::set_cgiInputWritten(size_t number)
{
	this->cgi_input_written = number;
}

void Client::update_activity() {
	this->last_activity = std::chrono::steady_clock::now();
}

void Client::appendToBufferRequest(std::string to_append){
	this->_request_buffer += to_append;
}

void Client::clearBuffer(){
	this->_request_buffer = "";
}