#ifndef CGI_HPP
#define CGI_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <vector>
// #include "Client.hpp"
#include "Server.hpp"

#define WRITE 1
#define READ 0

class Client;

class Cgi {
	private:
		int cgi_in[2];
		int cgi_out[2];
		std::vector<char*> env;
		std::vector<std::string> tmp_env;
		std::string path;
		std::string config_root;
		bool get;
		bool post;
		bool del;
		bool config_autoindex;
	public:
		Cgi();
		~Cgi();

		void start_cgi(Location location);
		int get_cgi_in(int pos);
		int get_cgi_out(int pos);
		void run_cgi(Client& client);
		void creating_cgi_env(Client &client);
		bool get_method_post() const;
		bool get_method_del() const;
		bool get_method_get() const;
		bool get_config_autoindex() const;

};

#endif