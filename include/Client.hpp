#include <iostream>
#include <sys/socket.h>
#include <unordered_map>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <memory>

#include "../include/ServerConfig.hpp"
// #include "../include/Cgi.hpp"

#pragma once

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"

//Example of a GET request:
// GET /index.html HTTP/1.1
// Host: localhost:8080
// User-Agent: curl/7.68.0
// Accept: */*

//Example for POST request:
// POST /submit-form HTTP/1.1
// Host: localhost:8080
// Content-Type: application/x-www-form-urlencoded
// Content-Length: 27

// name=John&age=30&city=NY

//GET and DELETE requests have the same structure and dont have a body.

//!!!!!!!!!!!!!If the request doesnt follow the HTTP standard, you can reject it with an appropriate error message (400 Bad Request).!!!!!!!!!!!
class Cgi;

class Client {
	private:
		int												_Client_socket; //socket descriptor
		std::string										_error_code;
		std::unordered_map<std::string, std::string>	_Client_RequestMap; //Request
		std::string										_response; //Response to be sent to the client
		ServerConfig&									_server_config; //Server configuration for the client
		bool isCGI;
		int CgiOutputfd;
		Cgi *cgi;
		std::string cgi_buffer;

	public:
		Client(int socket_fd, ServerConfig& server_config);
		~Client();
		// Client(const Client& other);
		Client& operator=(const Client& other);

		//Parsing methods
		int getpos(std::string str, std::string delimiter, int start);
		int parseClientRequest(std::string request);
		int parse_firstline(std::string request);
		int parse_header(std::string request);
		int parse_body(std::string request);

		//Building response methods
		int handle_get_request();
		// void handle_post_request();
		// void handle_delete_request();
		int handle_cgi_response(Cgi& cgi);

		bool is_method_allowed(const std::string& url_path, std::string method);
		std::string findRoot(const std::string& url_path);
		std::string build_body(const std::string& url_path, int flag = 0);
		std::string build_header(std::string body);
		std::string build_status_line(std::string status_code, std::string status_message);
		
		//Getters
		int get_Client_socket();
		std::string get_error_code();
		const std::unordered_map<std::string, std::string>& get_RequestMap() const;
		std::string get_Request(std::string key);
		std::string get_Response();
		bool get_isCgi();
		int get_cgiOutputfd();
		Cgi* get_cgi();

		//Setters
		void set_Client_socket(int socket_fd);
		void set_error_code(std::string error_code);
		void set_Request(std::string key, std::string value);
		void set_isCgi(bool value);
		void set_cgiOutputfd(int fd);
		void set_cgi(Cgi* cgi);
};
