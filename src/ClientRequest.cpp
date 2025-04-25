#include "../include/ClientRequest.hpp"

ClientRequest::ClientRequest(int socket_fd):_ClientRequest_socket(socket_fd){
	this->_ClientRequest_http_method = "";
	this->_ClientRequest_url_path = "";
	this->_ClientRequest_http_version = "";
	this->_ClientRequest_host = "";
	this->_ClientRequest_port = "";
	this->_ClientRequest_agent = "";
	this->_ClientRequest_accept = "";
	this->_ClientRequest_POST_body_content = "";
	this->_ClientRequest_POST_body_type = "";
	this->_ClientRequest_POST_body_length = "";
	std::cout << "Client Request received -- " << this->_ClientRequest_socket << std::endl; //Should add some kind of client identifier (Socket FD?);
}

ClientRequest::~ClientRequest(){
	//TO DO
	//Close the socket
	std::cout << "Client Request closed" << std::endl; //Should add some kind of client identifier (Socket FD?);
}

//Parsing methods
int ClientRequest::parseClientRequest(std::string request){
	// Returns -1 on error
	return 0;
}




//Getters
int ClientRequest::get_ClientRequest_socket(){
	return (this->_ClientRequest_socket);
}
std::string ClientRequest::get_ClientRequest_http_method(){
	return (this->_ClientRequest_http_method);
}
std::string ClientRequest::get_ClientRequest_url_path(){
	return (this->_ClientRequest_url_path);
}
std::string ClientRequest::get_ClientRequest_http_version(){
	return (this->_ClientRequest_http_version);
}
std::string ClientRequest::get_ClientRequest_host(){
	return (this->_ClientRequest_host);
}
std::string ClientRequest::get_ClientRequest_port(){
	return (this->_ClientRequest_port);
}
std::string ClientRequest::get_ClientRequest_agent(){
	return (this->_ClientRequest_agent);
}
std::string ClientRequest::get_ClientRequest_accept(){
	return (this->_ClientRequest_accept);
}
std::string ClientRequest::get_ClientRequest_POST_body_content(){
	return (this->_ClientRequest_POST_body_content);
}
std::string ClientRequest::get_ClientRequest_POST_body_type(){
	return (this->_ClientRequest_POST_body_type);
}
std::string ClientRequest::get_ClientRequest_POST_body_length(){
	return (this->_ClientRequest_POST_body_length);
}

//Setters
void ClientRequest::set_ClientRequest_socket(int socket_fd){
	this->_ClientRequest_socket = socket_fd;
}
void ClientRequest::set_ClientRequest_http_method(std::string http_method){
	this->_ClientRequest_http_method = http_method;
}
void ClientRequest::set_ClientRequest_url_path(std::string url_path){
	this->_ClientRequest_url_path = url_path;
}
void ClientRequest::set_ClientRequest_http_version(std::string http_version){
	this->_ClientRequest_http_version = http_version;
}
void ClientRequest::set_ClientRequest_host(std::string host){
	this->_ClientRequest_host = host;
}
void ClientRequest::set_ClientRequest_port(std::string port){
	this->_ClientRequest_port = port;
}
void ClientRequest::set_ClientRequest_agent(std::string agent){
	this->_ClientRequest_agent = agent;
}
void ClientRequest::set_ClientRequest_accept(std::string accept){
	this->_ClientRequest_accept = accept;
}
void ClientRequest::set_ClientRequest_POST_body_content(std::string body_content){
	this->_ClientRequest_POST_body_content = body_content;
}
void ClientRequest::set_ClientRequest_POST_body_type(std::string body_type){
	this->_ClientRequest_POST_body_type = body_type;
}
void ClientRequest::set_ClientRequest_POST_body_length(std::string body_length){
	this->_ClientRequest_POST_body_length = body_length;
}

