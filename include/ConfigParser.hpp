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

		bool is_file_extension_correct(std::string input);
		bool is_possible_use_file(std::string input);
		const std::vector<std::string>& getLines() const;

		bool is_server_config_load(const std::vector<std::string>& lines);
		// bool is_static_content_load(std::string str, ServerConfig& server, bool in_location_block);

		// Utils
		void remove_comments(std::string &str);
		void trim_spaces(std::string &str);
		bool is_host_valid(std::string value);
		bool is_digit_valid(std::string value);

};

#endif