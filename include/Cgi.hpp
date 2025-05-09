#ifndef CGI_HPP
#define CGI_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>

class Cgi {
	public:
		Cgi();
		~Cgi();

		void run_cgi(std::string path);

};
#endif