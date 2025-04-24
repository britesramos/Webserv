#include <iostream>

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

class ClientRequest {
	private:
		std::string _http_method_cr; //GET POST DELETE
		std::string _url_path_cr; // /index.html
		std::string _http_version_cr; //HTTP/1.1
		std::string _host_cr; //localhost:8080
		std::string _port_cr; //8080
		std::string _agent_cr; //browser or a tool like curl
		std::string _accept_cr;	//Indicates the type of content that the client can process
		/****For POST requests, the request includes a body with data to be sent to the server: ***/
		std::string _body_format_cr; 
		std::string _body_cr; 
		std::string _body_length_cr;

	public:
		ClientRequest();
		~ClientRequest();
};
