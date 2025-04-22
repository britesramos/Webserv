#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <iostream>
#include "ServerManager.hpp"
#include "ServerConfig.hpp"

class ConfigParser {
	private:
		std::vector<std::string> lines;
		ServerManager server;
	public:
		// ConfigParser();
		// ~ConfigParser();
		std::vector<std::string> tokenize_line(const std::string& line);
		bool is_file_extension_correct(std::string input);
		bool is_possible_use_file(std::string input);
		const std::vector<std::string>& getLines() const;

};

#endif