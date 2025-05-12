#ifndef CGI_HPP
#define CGI_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "Client.hpp"

class Cgi {
	private:
		pid_t pipefd[2];
		
	public:
		Cgi();
		~Cgi();

		void run_cgi(Client& client);

};
#endif