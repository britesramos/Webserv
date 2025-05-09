#include "../include/Cgi.hpp"

Cgi::Cgi()
{
	std::cout << "construct cgi" << std::endl;
}

Cgi::~Cgi()
{
	std::cout << "deconstruct cgi" << std::endl;
}


void Cgi::run_cgi(std::string path)
{
	if (path.find_last_of("/") == (path.size() - 1))
	{
		std::cout << " 							nao deveria estar aqui!" << std::endl;
	}
}

