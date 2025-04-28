#include <iostream>
#include <unordered_map>

#pragma once

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

class ClientRequest {
	private:
		int			_ClientRequest_socket; //socket descriptor
		std::unordered_map<std::string, std::string> _Client_RequestMap_header; //Headers
		std::unordered_map<std::string, std::string> _Client_RequestMap_body; //Body
		// std::string _ClientRequest_http_method; //GET POST DELETE
		// std::string _ClientRequest_url_path; // /index.html
		// std::string _ClientRequest_http_version; //HTTP/1.1
		// std::string _ClientRequest_host; //localhost:8080
		// std::string _ClientRequest_port; //8080
		// std::string _ClientRequest_agent; //browser or a tool like curl
		// std::string _ClientRequest_accept;	//Indicates the type of content that the client can process
		// /****For POST requests, the request includes a body with data to be sent to the server: ***/
		// std::string _ClientRequest_POST_body_content; 
		// std::string _ClientRequest_POST_body_type; 
		// std::string _ClientRequest_POST_body_length;

	public:
		ClientRequest(int socket_fd);
		~ClientRequest();

		//Parsing methods
		int parseClientRequest(std::string request);

		//Getters
		int get_ClientRequest_socket();
		std::string get_header(std::string key);
		std::string get_body(std::string key);
		// std::string get_ClientRequest_http_method();
		// std::string get_ClientRequest_url_path();
		// std::string get_ClientRequest_http_version();
		// std::string get_ClientRequest_host();
		// std::string get_ClientRequest_port();
		// std::string get_ClientRequest_agent();
		// std::string get_ClientRequest_accept();
		// std::string get_ClientRequest_POST_body_content();
		// std::string get_ClientRequest_POST_body_type();
		// std::string get_ClientRequest_POST_body_length();

		//Setters
		void set_ClientRequest_socket(int socket_fd);
		void set_header(std::string key, std::string value);
		void set_body(std::string key, std::string value);
		// void set_ClientRequest_http_method(std::string http_method);
		// void set_ClientRequest_url_path(std::string url_path);
		// void set_ClientRequest_http_version(std::string http_version);
		// void set_ClientRequest_host(std::string host);
		// void set_ClientRequest_port(std::string port);
		// void set_ClientRequest_agent(std::string agent);
		// void set_ClientRequest_accept(std::string accept);
		// void set_ClientRequest_POST_body_content(std::string body_content);
		// void set_ClientRequest_POST_body_type(std::string body_type);
		// void set_ClientRequest_POST_body_length(std::string body_length);

};
