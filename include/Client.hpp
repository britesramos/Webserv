#include <iostream>
#include <sys/socket.h>
#include <unordered_map>
#include <unistd.h>

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

class Client {
	private:
		int			_Client_socket; //socket descriptor
		std::unordered_map<std::string, std::string> _Client_RequestMap; //Request
		std::unordered_map<std::string, std::string> _Client_ResponseMap; //Response
		// std::string _Client_http_method; //GET POST DELETE
		// std::string _Client_url_path; // /index.html
		// std::string _Client_http_version; //HTTP/1.1
		// std::string _Client_host; //localhost:8080
		// std::string _Client_port; //8080
		// std::string _Client_agent; //browser or a tool like curl
		// std::string _Client_accept;	//Indicates the type of content that the client can process
		// /****For POST requests, the request includes a body with data to be sent to the server: ***/
		// std::string _Client_POST_body_content; 
		// std::string _Client_POST_body_type; 
		// std::string _Client_POST_body_length;

	public:
		Client(int socket_fd);
		~Client();
		Client(const Client& other);
		Client& operator=(const Client& other);

		//Parsing methods
		int getpos(std::string str, std::string delimiter, int start);
		int parseClientRequest(std::string request);
		int parse_firstline(std::string request);
		int parse_header(std::string request);
		int parse_body(std::string request);
		
		//Getters
		int get_Client_socket();
		const std::unordered_map<std::string, std::string>& get_RequestMap() const;
		const std::unordered_map<std::string, std::string>& get_ResponseMap() const;
		std::string get_Request(std::string key);
		std::string get_Response(std::string key);

		//Setters
		void set_Client_socket(int socket_fd);
		void set_Request(std::string key, std::string value);
		void set_Response(std::string key, std::string value);

};
