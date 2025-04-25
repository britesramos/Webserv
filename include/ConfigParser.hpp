#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <iostream>
#include <unordered_map>
#include <fstream>
#include <vector>
#include "ServerConfig.hpp"

class ConfigParser {
	private:
		std::vector<std::string> lines;
		std::vector<ServerConfig> servers;
	public:
		// ConfigParser();
		// ~ConfigParser();
		void addServer(ServerConfig &server);
		const std::vector<ServerConfig>& getServer() const;

		// std::vector<std::string> tokenize_line(const std::string& line);
		bool is_file_extension_correct(std::string input);
		bool is_possible_use_file(std::string input);
		const std::vector<std::string>& getLines() const;

		bool is_map_filled(const std::vector<std::string>& lines);

};

#endif